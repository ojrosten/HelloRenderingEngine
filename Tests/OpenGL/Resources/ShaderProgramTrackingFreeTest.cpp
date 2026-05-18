////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramTrackingFreeTest.hpp"

#include "OpenGL/ResourceInfrastructure/ResourceHandleTestingUtilities.hpp"

#include "avocet/Core/Utilities/ContainerUtilities.hpp"
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

    namespace
    {
        [[nodiscard]]
        curlew::window_config make_window_config(std::vector<std::string>& calls) {
            return curlew::make_call_logging_window_config({.width{1}, .height{1}}, calls, [](const agl::decorator_data& data) { return data.fn_name == "UseProgram"; });
        }

        agl::resource_handle get_current_program_index(const agl::decorated_context& ctx) {
            GLint param{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, GL_CURRENT_PROGRAM, &param);
            if(param < 0)
                throw std::runtime_error{std::format("Negative program index: {}", param)};

            return agl::resource_handle{static_cast<GLuint>(param)};
        }


        using opt_latch_ref = std::optional<std::reference_wrapper<std::latch>>;

        constexpr opt_latch_ref no_latch{};

        using program_handles = shader_program_tracking_free_test::program_handles;

        struct resource_artefacts {
            agl::resource_handle handle;
            std::optional<agl::shader_program> opt_shader_prog{};
        };

        enum class extend_resource_lifetime : bool { no, yes };

        [[nodiscard]]
        resource_artefacts make_and_use_shader_program(const curlew::window& w,
                                                       const fs::path& shaderDir,
                                                       opt_latch_ref entryLatch,
                                                       opt_latch_ref exitLatch,
                                                       extend_resource_lifetime extendResourceLifetime)
        {
            const auto& ctx{w.context()};

            agl::shader_program sp{ctx, shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
            if(entryLatch) entryLatch->get().arrive_and_wait();

            sp.use();
            sp.use();

            auto handle{get_current_program_index(ctx)};

            if(exitLatch) exitLatch->get().arrive_and_wait();

            return {
                std::move(handle),
                extendResourceLifetime == extend_resource_lifetime::yes ? std::optional{std::move(sp)}
                                                                        : std::optional<agl::shader_program>{}
            };
        }

        [[nodiscard]]
        program_handles make_and_use_shader_program(curlew::window w, const fs::path& shaderDir) {
            return {make_and_use_shader_program(w, shaderDir, no_latch, no_latch, extend_resource_lifetime::no).handle, get_current_program_index(w.context())};
        }

        [[nodiscard]]
        resource_artefacts make_and_use_extended_life_shader_program(const curlew::window& w, const fs::path& shaderDir) {
            return make_and_use_shader_program(w, shaderDir, no_latch, no_latch, extend_resource_lifetime::yes);
        }

        using task_t = std::packaged_task<program_handles()>;

        [[nodiscard]]
        task_t make_shader_program_task(curlew::window& w, const fs::path& shaderDir, opt_latch_ref entryLatch, opt_latch_ref exitLatch) {
            curlew::test_window_manager::detach_current_context();

            return task_t{
                [&, entryLatch, exitLatch]() -> program_handles {
                    w.make_context_current();
                    return {make_and_use_shader_program(w, shaderDir, entryLatch, exitLatch, extend_resource_lifetime::no).handle, get_current_program_index(w.context())};
                }
            };
        }

        [[nodiscard]]
        program_handles make_and_use_shader_program_threaded(curlew::window w, const fs::path& shaderDir) {
            auto task{make_shader_program_task(w, shaderDir, no_latch, no_latch)};

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

    [[nodiscard]]
    std::filesystem::path shader_program_tracking_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_tracking_free_test::run_tests()
    {
        check_serial_tracking_non_overlapping_lifetimes();
        check_serial_tracking_overlapping_lifetimes();

        check_threaded_tracking_non_overlapping_lifetimes();
        check_threaded_tracking_overlapping_lifetimes();
    }

    void shader_program_tracking_free_test::check_serial_tracking_non_overlapping_lifetimes()
    {
        gpu_data data0{}, data1{};

        const auto shaderDir{working_materials()};
        data0.prog = make_and_use_shader_program(create_window(make_window_config(data0.calls)), shaderDir),
        data1.prog = make_and_use_shader_program(create_window(make_window_config(data1.calls)), shaderDir);

        check_program_data("Serial non-overlapping lifetimes", data0, data1);
    }

    void shader_program_tracking_free_test::check_serial_tracking_overlapping_lifetimes()
    {
        const auto shaderDir{working_materials()};

        gpu_data data0{};
        auto win0{create_window(make_window_config(data0.calls))};
        auto [handle0, sp0] {make_and_use_extended_life_shader_program(win0, shaderDir)};
        data0.prog.active = std::move(handle0);

        gpu_data data1{};
        auto win1{create_window(make_window_config(data1.calls))};
        auto [handle1, sp1] {make_and_use_extended_life_shader_program(win1, shaderDir)};
        data1.prog.active = std::move(handle1);

        check_program_data("Serial overlapping lifetimes", data0, data1);
    }

    void shader_program_tracking_free_test::check_threaded_tracking_non_overlapping_lifetimes()
    {
        gpu_data data0{}, data1{};

        const auto shaderDir{working_materials()};
        data0.prog = make_and_use_shader_program_threaded(create_window(make_window_config(data0.calls)), shaderDir),
        data1.prog = make_and_use_shader_program_threaded(create_window(make_window_config(data1.calls)), shaderDir);

        check_program_data("Threaded non-overlapping lifetimes", data0, data1);
    }

    void shader_program_tracking_free_test::check_threaded_tracking_overlapping_lifetimes()
    {
        const auto shaderDir{working_materials()};

        std::array<gpu_data, 4> data{};

        auto windows{to_array(std::span{data}, [this](gpu_data& d) { return create_window(make_window_config(d.calls)); })};

        std::latch entryLatch{data.size()}, exitLatch{data.size()};
        auto tasks{to_array(std::span{windows}, [&](curlew::window& win) { return make_shader_program_task(win, shaderDir, entryLatch, exitLatch); })};

        auto futures{to_array(std::span{tasks}, [](task_t& t) { return t.get_future(); })};

        auto workers{to_array(std::span{tasks}, [](task_t& t) { return std::jthread{std::move(t)}; })};

        for(auto [gpuData, fut] : std::views::zip(data, futures)) {
            gpuData.prog = fut.get();
        }

        for(auto i  : std::views::iota(1uz, data.size()))
            check_program_data("Threaded overlapping lifetimes", data[0], data[i]);
    }

    void shader_program_tracking_free_test::check_program_data(std::string_view tag, const gpu_data& data0, const gpu_data& data1)
    {
        check_program_indices(tag, data0.prog, data1.prog);

        auto buildPrediction{
            [](const program_handles& prog) {
                std::vector<std::string> calls{"UseProgram"};
                if(prog.after_destruction)
                    calls.push_back("UseProgram");

                return calls;
            }
        };

        check(equivalence, "", data0.calls, buildPrediction(data0.prog));
        check(equivalence, "", data1.calls, buildPrediction(data1.prog));
    }

    void shader_program_tracking_free_test::check_reset_after_destruction(std::string_view tag, const program_handles& prog)
    {
        if(prog.after_destruction)
            check(equality, make_description(tag, "Program should be reset to 0"), prog.after_destruction.value(), agl::resource_handle{});
    }

    void shader_program_tracking_free_test::check_program_indices(std::string_view tag, const program_handles& prog0, const program_handles& prog1)
    {
        check(make_description(tag, "prog0 should not be null"), prog0.active != agl::resource_handle{});
        check(make_description(tag, "prog1 should not be null"), prog1.active != agl::resource_handle{});

        check(equality, make_description(tag, "Assumption required for sensitivity to: program 0 utilization accidentally suppressing program 1 utilization"), prog0.active, prog1.active);

        check_reset_after_destruction(tag, prog0);
        check_reset_after_destruction(tag, prog1);
    }
}
