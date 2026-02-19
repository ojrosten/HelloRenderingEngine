////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramTrackingFreeTest.hpp"

#include "ResourceHandleTestingUtilities.hpp"

#include "avocet/OpenGL/Resources/ShaderProgram.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

#include <future>
#include <thread>
#include <latch>

namespace avocet::testing
{
    namespace agl = avocet::opengl;
    namespace fs = std::filesystem;

    namespace
    {
        [[nodiscard]]
        curlew::window_config make_window_config(std::vector<std::string>& loggedCalls) {
            return {
                .extent{.width{1}, .height{1}},
                .name{},
                .hiding{curlew::window_hiding_mode::on},
                .debug_mode{agl::debugging_mode::dynamic},
                .prologue{},
                .epilogue{agl::standard_error_checker{agl::num_messages{10}, agl::default_debug_info_processor{}}},
                .compensate{agl::attempt_to_compensate_for_driver_bugs::yes},
                .samples{1}
            };
        }

        agl::resource_handle get_current_program_index(const agl::decorated_context& ctx) {
            GLint param{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, GL_CURRENT_PROGRAM, &param);
            if(param < 0)
                throw std::runtime_error{std::format("Negative program index: {}", param)};

            return agl::resource_handle{static_cast<GLuint>(param)};
        }

        agl::resource_handle make_and_use_shader_program(curlew::window w,const fs::path& shaderDir) {
            const auto& ctx{w.context()};

            agl::shader_program sp{ctx, shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
            sp.use();

            return get_current_program_index(ctx);
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

        check_parallel_tracking_non_overlapping_lifetimes();
        check_parallel_tracking_overlapping_lifetimes();
    }

    void shader_program_tracking_free_test::check_serial_tracking_non_overlapping_lifetimes()
    {
        gpu_data data0{}, data1{};

        const auto shaderDir{working_materials()};
        data0.prog = make_and_use_shader_program(create_window(make_window_config(data0.calls)), shaderDir),
        data1.prog = make_and_use_shader_program(create_window(make_window_config(data1.calls)), shaderDir);

        check_program_indices("Serial non-overlapping lifetimes", data0, data1);
    }

    void shader_program_tracking_free_test::check_serial_tracking_overlapping_lifetimes()
    {

    }

    void shader_program_tracking_free_test::check_parallel_tracking_non_overlapping_lifetimes()
    {

    }

    void shader_program_tracking_free_test::check_parallel_tracking_overlapping_lifetimes()
    {

    }

    void shader_program_tracking_free_test::check_program_indices(std::string_view tag, const gpu_data& data0, const gpu_data& data1)
    {
        check(make_description(tag, "prog0 should not be null"), data0.prog != agl::resource_handle{});
        check(make_description(tag, "prog1 should not be null"), data1.prog != agl::resource_handle{});

        check(equality, make_description(tag, "Assumption required for sensitivity to: program 0 utilization accidentally suppressing program 1 utilization"), data0.prog, data1.prog);
    }
}
