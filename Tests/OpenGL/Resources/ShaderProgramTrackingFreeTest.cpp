////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramTrackingFreeTest.hpp"

#include "ResourceHandleTestingUtilities.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

#include <thread>
#include <future>

namespace avocet::testing
{
    namespace agl = avocet::opengl;
    namespace fs = std::filesystem;

    namespace
    {
        agl::resource_handle get_current_program_index(const agl::decorated_context& ctx) {
            GLint param{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, GL_CURRENT_PROGRAM, &param);
            if(param < 0)
                throw std::runtime_error{std::format("Negative program index: {}", param)};

            return agl::resource_handle{static_cast<GLuint>(param)};
        }

        agl::resource_handle make_and_use_shader_program(const curlew::window& w,const fs::path& shaderDir) {
            const auto& ctx{w.context()};

            agl::shader_program sp{ctx, shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
            sp.use();

            return get_current_program_index(ctx);
        }

        agl::resource_handle make_and_use_shader_program_threaded(curlew::window& w, const fs::path& shaderDir) {
            curlew::test_window_manager::detach_current_context();

            std::packaged_task<agl::resource_handle()>
                task{
                    [&w, &shaderDir]() {
                        w.make_context_current();
                        return make_and_use_shader_program(w, shaderDir);
                    }
            };

            auto fut{task.get_future()};

            {
                std::jthread executor{std::move(task)};
            }

            return fut.get();
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
        check_serial_tracking_non_overlapping_shader_prog_lifetimes();
        check_serial_tracking_overlapping_shader_prog_lifetimes();

        check_parallel_tracking_non_overlapping_shader_prog_lifetimes();
    }

    void shader_program_tracking_free_test::check_serial_tracking_non_overlapping_shader_prog_lifetimes()
    {
        const auto shaderDir{working_materials()};
        const auto prog0{make_and_use_shader_program(create_window({.hiding{curlew::window_hiding_mode::on}}), shaderDir)},
                   prog1{make_and_use_shader_program(create_window({.hiding{curlew::window_hiding_mode::on}}), shaderDir)};

        check_program_indices("Serial non-overlapping lifetimes", prog0, prog1);
    }

    void shader_program_tracking_free_test::check_serial_tracking_overlapping_shader_prog_lifetimes()
    {
        const auto shaderDir{working_materials()};
        const auto win0{create_window({.hiding{curlew::window_hiding_mode::on}})};
        agl::shader_program sp1{win0.context(), shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
        sp1.use();
        const auto prog0{get_current_program_index(win0.context())};

        const auto win1{create_window({.hiding{curlew::window_hiding_mode::on}})};
        agl::shader_program sp2{win1.context(), shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
        sp2.use();
        const auto prog1{get_current_program_index(win1.context())};

        check_program_indices("Serial overlapping lifetimes", prog0, prog1);
    }

     void shader_program_tracking_free_test::check_parallel_tracking_non_overlapping_shader_prog_lifetimes()
    {
        const auto shaderDir{working_materials()};
        auto win0{create_window({.hiding{curlew::window_hiding_mode::on}})},
             win1{create_window({.hiding{curlew::window_hiding_mode::on}})};

        const auto prog0{make_and_use_shader_program_threaded(win0, shaderDir)},
                   prog1{make_and_use_shader_program_threaded(win1, shaderDir)};

        check_program_indices("Parallel non-overlapping lifetimes", prog0, prog1);
    }

    void shader_program_tracking_free_test::check_program_indices(std::string_view tag, const avocet::opengl::resource_handle& prog0, const avocet::opengl::resource_handle& prog1)
    {
        check(make_description(tag, "prog0 should not be null"), prog0 != agl::resource_handle{});
        check(make_description(tag, "prog1 should not be null"), prog1 != agl::resource_handle{});

        check(equality, make_description(tag, "Assumption required for sensitivity to: program 0 utilization accidentally suppressing program 1 utilization"), prog0, prog1);
    }
}
