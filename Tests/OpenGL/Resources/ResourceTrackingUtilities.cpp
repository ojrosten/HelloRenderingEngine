////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ResourceTrackingUtilities.hpp"
#include "OpenGL/ResourceInfrastructure/ResourceHandleTestingUtilities.hpp"

#include "avocet/OpenGL/Context/GLGetters.hpp"
#include "avocet/OpenGL/Resources/Framebuffer.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/WindowConfigurations.hpp"

#include <future>
#include <thread>
#include <latch>
#include <optional>
#include <ranges>

namespace avocet::testing
{
    namespace agl = avocet::opengl;
    namespace fs = std::filesystem;

    namespace {
        [[nodiscard]]
        curlew::window_config make_window_config(std::vector<std::string>& calls, std::string_view expectedGPUCall) {
            return curlew::make_call_logging_window_config({.width{1}, .height{1}}, calls, [expectedGPUCall](const agl::decorator_data& data) { return data.fn_name == expectedGPUCall; });
        }

        [[nodiscard]]
        agl::resource_handle get_current_resource_index(const agl::decorated_context& ctx, GLenum glName) {
            GLint param{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, glName, &param);
            if(param < 0)
                throw std::runtime_error{std::format("Negative resource index: {}", param)};

            return agl::resource_handle{static_cast<GLuint>(param)};
        }


        using opt_latch_ref = std::optional<std::reference_wrapper<std::latch>>;

        constexpr opt_latch_ref no_latch{};

        using resource_handles = resource_tracking_test_base::resource_handles;

        template<class Resource>
        struct resource_artefacts {
            agl::resource_handle handle;
            std::optional<Resource> opt_shader_prog{};
        };

        enum class extend_resource_lifetime : bool { no, yes };

        template<class Creator, class Utilizer, class Resource=std::invoke_result_t<Creator, agl::resourceful_context>>
        [[nodiscard]]
        resource_artefacts<Resource> make_and_use_resource(const curlew::window& w,
                                                           GLenum glName,
                                                           Creator creator,
                                                           Utilizer utilizer,
                                                           opt_latch_ref entryLatch,
                                                           opt_latch_ref exitLatch,
                                                           extend_resource_lifetime extendResourceLifetime)
        {
            const auto& ctx{w.context()};

            auto resource{creator(ctx)};
            if(entryLatch) entryLatch->get().arrive_and_wait();

            utilizer(resource);
            utilizer(resource);

            auto handle{get_current_resource_index(ctx, glName)};

            if(exitLatch) exitLatch->get().arrive_and_wait();

            return {
                std::move(handle),
                extendResourceLifetime == extend_resource_lifetime::yes ? std::optional{std::move(resource)}
                                                                        : std::optional<Resource>{}
            };
        }

        template<class Creator, class Utilizer>
        [[nodiscard]]
        resource_handles make_and_use_resource(curlew::window w, GLenum glName, Creator creator, Utilizer utilizer) {
            return {make_and_use_resource(w, glName, creator, utilizer, no_latch, no_latch, extend_resource_lifetime::no).handle, get_current_resource_index(w.context(), glName)};
        }

        template<class Creator, class Utilizer, class Resource = std::invoke_result_t<Creator, agl::resourceful_context>>
        [[nodiscard]]
        resource_artefacts<Resource> make_and_use_extended_life_resource(const curlew::window& w, GLenum glName, Creator creator, Utilizer utilizer) {
            return make_and_use_resource(w, glName, creator, utilizer, no_latch, no_latch, extend_resource_lifetime::yes);
        }

        using task_t = std::packaged_task<resource_handles()>;

        template<class Creator, class Utilizer>
        [[nodiscard]]
        task_t make_resource_task(curlew::window& w, GLenum glName, Creator creator, Utilizer utilizer, opt_latch_ref entryLatch, opt_latch_ref exitLatch) {
            curlew::test_window_manager::detach_current_context();

            return task_t{
                [=,&w]() -> resource_handles {
                    w.make_context_current();
                    return {make_and_use_resource(w, glName, creator, utilizer, entryLatch, exitLatch, extend_resource_lifetime::no).handle, get_current_resource_index(w.context(), glName)};
                }
            };
        }

        template<class Creator, class Utilizer>
        [[nodiscard]]
        resource_handles make_and_use_resource_threaded(curlew::window w, GLenum glName, Creator creator, Utilizer utilizer) {
            auto task{make_resource_task(w, glName, creator, utilizer, no_latch, no_latch)};

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

    template<class Resource>
    void resource_tracking_test<Resource>::check_serial_tracking_non_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer)
    {
        gpu_data data0{}, data1{};

        data0.resource = make_and_use_resource(create_window(make_window_config(data0.calls, expectedGPUCall)), glName, creator, utilizer);
        data1.resource = make_and_use_resource(create_window(make_window_config(data1.calls, expectedGPUCall)), glName, creator, utilizer);

        check_resource_data("Serial non-overlapping lifetimes", expectedGPUCall, data0, data1);
    }

    template<class Resource>
    void resource_tracking_test<Resource>::check_serial_tracking_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer)
    {
        gpu_data data0{};
        auto win0{create_window(make_window_config(data0.calls, expectedGPUCall))};
        auto [handle0, sp0] {make_and_use_extended_life_resource(win0, glName, creator, utilizer)};
        data0.resource.active = std::move(handle0);

        gpu_data data1{};
        auto win1{create_window(make_window_config(data1.calls, expectedGPUCall))};
        auto [handle1, sp1] {make_and_use_extended_life_resource(win1, glName, creator, utilizer)};
        data1.resource.active = std::move(handle1);

        check_resource_data("Serial overlapping lifetimes", expectedGPUCall, data0, data1);
    }

    template<class Resource>
    void resource_tracking_test<Resource>::check_threaded_tracking_non_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer)
    {
        gpu_data data0{}, data1{};

        data0.resource = make_and_use_resource_threaded(create_window(make_window_config(data0.calls, expectedGPUCall)), glName, creator, utilizer);
        data1.resource = make_and_use_resource_threaded(create_window(make_window_config(data1.calls, expectedGPUCall)), glName, creator, utilizer);

        check_resource_data("Threaded non-overlapping lifetimes", expectedGPUCall, data0, data1);
    }

    template<class Resource>
    void resource_tracking_test<Resource>::check_threaded_tracking_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer)
    {
        std::array<gpu_data, 4> data{};

        auto windows{to_array(std::span{data}, [this, expectedGPUCall](gpu_data& d) { return create_window(make_window_config(d.calls, expectedGPUCall)); })};

        std::latch entryLatch{data.size()}, exitLatch{data.size()};
        auto tasks{to_array(std::span{windows}, [=,&entryLatch, &exitLatch](curlew::window& win) { return make_resource_task(win, glName, creator, utilizer, entryLatch, exitLatch); })};

        auto futures{to_array(std::span{tasks}, [](task_t& t) { return t.get_future(); })};

        auto workers{to_array(std::span{tasks}, [](task_t& t) { return std::jthread{std::move(t)}; })};

        for(auto [gpuData, fut] : std::views::zip(data, futures)) {
            gpuData.resource = fut.get();
        }

        for(auto i : std::views::iota(1uz, data.size()))
            check_resource_data("Threaded overlapping lifetimes", expectedGPUCall, data[0], data[i]);
    }

    template class resource_tracking_test<agl::shader_program>;
    template class resource_tracking_test<agl::framebuffer_object>;
}