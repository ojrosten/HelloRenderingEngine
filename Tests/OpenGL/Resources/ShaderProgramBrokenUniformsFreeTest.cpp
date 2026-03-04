////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramBrokenUniformsFreeTest.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

namespace avocet::testing
{
    using namespace curlew;

    [[nodiscard]]
    std::filesystem::path shader_program_broken_uniforms_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_broken_uniforms_free_test::run_tests()
    {
        test_broken_uniforms(create_window({.hiding{window_hiding_mode::on}}));
        test_broken_uniforms(create_window({.hiding{window_hiding_mode::on}, .debug_mode{agl::debugging_mode::basic}}));
    }

    void shader_program_broken_uniforms_free_test::test_broken_uniforms(const curlew::window& win)
    {
        const auto shaderDir{working_materials()};

        agl::shader_program sp{win.context(), shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};

        check_exception_thrown<std::runtime_error>(
            "Trying to set a non-existent uniform",
            [&sp]() { return sp.set_uniform("foo", 1.0f); }
        );

        check_exception_thrown<std::runtime_error>(
            "Trying to get a non-existent uniform",
            [&sp]() { return sp.get_uniform<GLfloat>("foo"); }
        );
    }
}
