////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "ResourceTrackingUtilities.hpp"

#include "OpenGL/ResourceInfrastructure/ResourceHandleTestingUtilities.hpp"

#include "avocet/Core/Utilities/ContainerUtilities.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"
#include "avocet/OpenGL/Resources/Framebuffer.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include <future>
#include <thread>
#include <latch>
#include <optional>
#include <ranges>

namespace avocet::testing
{
    template resource_tracking_test<opengl::shader_program>;
    template resource_tracking_test<opengl::framebuffer_object>;

    namespace {
        class call_logger {
            std::string m_ExpectedCall{};
            std::vector<std::string>* m_Calls{};
        public:
            call_logger(std::string_view expectedGPUCall, std::vector<std::string>& calls)
                : m_ExpectedCall{std::string{expectedGPUCall}}
                , m_Calls{&calls}
            {
            }

            void operator()(const opengl::context&, const opengl::decorator_data& data) {
                if(data.fn_name == m_ExpectedCall)
                    m_Calls->push_back(std::string{data.fn_name});
            }
        };

        [[nodiscard]]
        curlew::window_config make_window_config(std::string_view expectedGPUCall, std::vector<std::string>& calls) {
            return {
                .extent{.width{1}, .height{1}},
                .name{},
                .hiding{curlew::window_hiding_mode::on},
                .debug_mode{opengl::debugging_mode::dynamic},
                .prologue{call_logger{expectedGPUCall, calls}},
                .epilogue{opengl::standard_error_checker{opengl::num_messages{10}, opengl::default_debug_info_processor{}}},
                .compensate{opengl::attempt_to_compensate_for_driver_bugs::yes},
                .samples{1}
            };
        }

        [[nodiscard]]
        opengl::resource_handle get_current_resource_index(const opengl::decorated_context& ctx, opengl::int_names glGetName) {
            return opengl::resource_handle{checked_conversion_to<GLuint>(opengl::get(ctx, glGetName))};
        }

        using opt_latch_ref = std::optional<std::reference_wrapper<std::latch>>;

        constexpr opt_latch_ref no_latch{};

        template<std::invocable<opengl::resourceful_context> Creator, std::invocable<std::invoke_result_t<Creator, opengl::resourceful_context>> Utilizer>
        [[nodiscard]]
        opengl::resource_handle make_and_use_resource(const curlew::window& w, opengl::int_names glGetName, Creator creator, Utilizer utilizer, opt_latch_ref entryLatch, opt_latch_ref exitLatch) {
            const auto& ctx{w.context()};

            auto resource{creator(ctx)};
            if(entryLatch) entryLatch->get().arrive_and_wait();
            utilizer(resource);
            utilizer(resource);

            auto handle{get_current_resource_index(ctx, glGetName)};
            if(exitLatch) exitLatch->get().arrive_and_wait();

            return handle;
        }

        using resource_handles = resource_tracking_test_base::resource_handles;

        template<std::invocable<opengl::resourceful_context> Creator, std::invocable<std::invoke_result_t<Creator, opengl::resourceful_context>> Utilizer>
        [[nodiscard]]
        resource_handles make_and_utilize_resource(curlew::window w, opengl::int_names glGetName, Creator creator, Utilizer utilizer) {
            return {make_and_use_resource(w, glGetName, creator, utilizer, no_latch, no_latch), get_current_resource_index(w.context(), glGetName)};
        }

        using task_t = std::packaged_task<resource_handles()>;

        template<std::invocable<opengl::resourceful_context> Creator, std::invocable<std::invoke_result_t<Creator, opengl::resourceful_context>> Utilizer>
        [[nodiscard]]
        task_t make_resource_task(curlew::window& w, opengl::int_names glGetName, Creator creator, Utilizer utilizer, opt_latch_ref entryLatch, opt_latch_ref exitLatch) {
            curlew::test_window_manager::detach_current_context();

            return task_t{
                [=,&w]() -> resource_handles {
                    w.make_context_current();
                    return {make_and_use_resource(w, glGetName, creator, utilizer, entryLatch, exitLatch), get_current_resource_index(w.context(), glGetName)};
                }
            };
        }

        template<std::invocable<opengl::resourceful_context> Creator, std::invocable<std::invoke_result_t<Creator, opengl::resourceful_context>> Utilizer>
        [[nodiscard]]
        resource_handles make_and_utilize_resource_threaded(curlew::window w, opengl::int_names glGetName, Creator creator, Utilizer utilizer) {
            auto task{make_resource_task(w, glGetName, creator, utilizer, no_latch, no_latch)};

            auto future{task.get_future()};

            std::jthread worker{std::move(task)};

            return future.get();
        }

        [[nodiscard]]
        std::string make_description(std::string_view tag, std::string_view description)
        {
            return std::format("{}\n{}", tag, description);
        }
    }

    template<class Resource>
    void resource_tracking_test<Resource>::check_serial_tracking_non_overlapping_lifetimes(std::string_view expectedGPUCall, opengl::int_names glGetName, creator_type creator, utilizer_type utilizer)
    {
        gpu_data data0{}, data1{};

        data0.resource = make_and_utilize_resource(create_window(make_window_config(expectedGPUCall, data0.calls)), glGetName, creator, utilizer);
        data1.resource = make_and_utilize_resource(create_window(make_window_config(expectedGPUCall, data1.calls)), glGetName, creator, utilizer);

        check_resource_data("Serial non-overlapping lifetimes", expectedGPUCall, data0, data1);
    }

    template<class Resource>
    void resource_tracking_test<Resource>::check_serial_tracking_overlapping_lifetimes(std::string_view expectedGPUCall, opengl::int_names glGetName, creator_type creator, utilizer_type utilizer)
    {
        gpu_data data0{};
        auto win0{create_window(make_window_config(expectedGPUCall, data0.calls))};
        auto resource0{creator(win0.context())};
        utilizer(resource0);
        utilizer(resource0);
        data0.resource.active = get_current_resource_index(win0.context(), glGetName);

        gpu_data data1{};
        auto win1{create_window(make_window_config(expectedGPUCall, data1.calls))};
        auto resource1{creator(win1.context())};
        utilizer(resource1);
        utilizer(resource1);
        data1.resource.active = get_current_resource_index(win1.context(), glGetName);

        check_resource_data("Serial overlapping lifetimes", expectedGPUCall, data0, data1);
    }

    template<class Resource>
    void resource_tracking_test<Resource>::check_threaded_tracking_non_overlapping_lifetimes(std::string_view expectedGPUCall, opengl::int_names glGetName, creator_type creator, utilizer_type utilizer)
    {
        gpu_data data0{}, data1{};

        const auto shaderDir{working_materials()};
        data0.resource = make_and_utilize_resource_threaded(create_window(make_window_config(expectedGPUCall, data0.calls)), glGetName, creator, utilizer),
        data1.resource = make_and_utilize_resource_threaded(create_window(make_window_config(expectedGPUCall, data1.calls)), glGetName, creator, utilizer);

        check_resource_data("Threaded non-overlapping lifetimes", expectedGPUCall, data0, data1);
    }

    template<class Resource>
    void resource_tracking_test<Resource>::check_threaded_tracking_overlapping_lifetimes(std::string_view expectedGPUCall, opengl::int_names glGetName, creator_type creator, utilizer_type utilizer)
    {
        std::array<gpu_data, 4> data{};

        auto windows{to_array(std::span{data}, [this, expectedGPUCall](gpu_data& d) { return create_window(make_window_config(expectedGPUCall, d.calls)); })};

        std::latch entryLatch{data.size()}, exitLatch{data.size()};
        auto tasks{to_array(std::span{windows}, [&](curlew::window& win) { return make_resource_task(win, glGetName, creator, utilizer, entryLatch, exitLatch); })};

        auto futures{to_array(std::span{tasks}, [](task_t& t) { return t.get_future(); })};

        auto workers{to_array(std::span{tasks}, [](task_t& t) { return std::jthread{std::move(t)}; })};

        for(auto e : std::views::zip(data, futures)) {
            std::get<0>(e).resource = std::get<1>(e).get();
        }

        for(auto i : std::views::iota(1uz, data.size()))
            check_resource_data("Threaded overlapping lifetimes", expectedGPUCall, data[0], data[i]);
    }

    void resource_tracking_test_base::check_resource_data(std::string_view tag, std::string_view expectedGPUCall, const gpu_data& data0, const gpu_data& data1)
    {
        check_resource_indices(tag, data0.resource, data1.resource);

        auto buildPrediction{
            [expectedGPUCall](const resource_handles& resource) {
                std::vector<std::string> calls{std::string{expectedGPUCall}};
                if(resource.after_destruction)
                    calls.push_back(std::string{expectedGPUCall});

                return calls;
            }
        };

        check(equivalence, tag, data0.calls, buildPrediction(data0.resource));
        check(equivalence, tag, data1.calls, buildPrediction(data1.resource));
    }

    void resource_tracking_test_base::check_reset_after_destruction(std::string_view tag, const resource_handles& resource)
    {
        if(resource.after_destruction)
            check(equality, make_description(tag, "Resource should be reset to 0"), resource.after_destruction.value(), opengl::resource_handle{});
    }

    void resource_tracking_test_base::check_resource_indices(std::string_view tag, const resource_handles& resource0, const resource_handles& resource1)
    {
        check(make_description(tag, "Resource 0 should not be null"), resource0.active != opengl::resource_handle{});
        check(make_description(tag, "Resource 1 should not be null"), resource1.active != opengl::resource_handle{});

        check(equality, make_description(tag, "Assumption required for sensitivity to: Resource 0 utilization accidentally suppressing Resource 1 utilization"), resource0.active, resource1.active);

        check_reset_after_destruction(tag, resource0);
        check_reset_after_destruction(tag, resource1);
    }
}