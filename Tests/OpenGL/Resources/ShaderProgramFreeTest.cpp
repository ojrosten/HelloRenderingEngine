////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramFreeTest.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path shader_program_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_free_test::run_tests()
    {
        using namespace curlew;
        auto win{create_window({.hiding{window_hiding_mode::on}})};

        const auto shaderDir{working_materials()};

        agl::shader_program
            sp0{win.context(), shaderDir / "Identity.vs", shaderDir / "MixedBag.fs"},
            sp1{win.context(), shaderDir / "Identity.vs", shaderDir / "MixedBag.fs"};

        check_exception_thrown<std::runtime_error>(
            "Trying to set a non-existent uniform",
            [&sp0]() { return sp0.set_uniform("goo_f", 1.0f); }
        );

        check_exception_thrown<std::runtime_error>(
            "Trying to get a non-existent uniform",
            [&sp0]() { return sp0.get_uniform<GLfloat>("goo_f"); }
        );

        check(equality, "", sp0.get_uniform<GLfloat>("foo_f"), 1.0f);
        check(equality, "", sp0.get_uniform<GLfloat>("bar_f"), 2.0f);
        check(equality, "", sp1.get_uniform<GLfloat>("foo_f"), 1.0f);
        check(equality, "", sp1.get_uniform<GLfloat>("bar_f"), 2.0f);

        check(equality, "", sp0.get_uniform<GLfloat, 2>("foo_f2"), std::array{ 1.f, -1.f});
        check(equality, "", sp0.get_uniform<GLfloat, 2>("bar_f2"), std::array{-2.f,  2.f});
        check(equality, "", sp1.get_uniform<GLfloat, 2>("foo_f2"), std::array{ 1.f, -1.f});
        check(equality, "", sp1.get_uniform<GLfloat, 2>("bar_f2"), std::array{-2.f,  2.f});

        check(equality, "", sp0.get_uniform<GLfloat, 3>("foo_f3"), std::array{ 1.f, -1.f,  1.f});
        check(equality, "", sp0.get_uniform<GLfloat, 3>("bar_f3"), std::array{-2.f,  2.f, -2.f});
        check(equality, "", sp1.get_uniform<GLfloat, 3>("foo_f3"), std::array{ 1.f, -1.f,  1.f});
        check(equality, "", sp1.get_uniform<GLfloat, 3>("bar_f3"), std::array{-2.f,  2.f, -2.f});

        check(equality, "", sp0.get_uniform<GLfloat, 4>("foo_f4"), std::array{ 1.f, -1.f,  1.f, -1.f});
        check(equality, "", sp0.get_uniform<GLfloat, 4>("bar_f4"), std::array{-2.f,  2.f, -2.f,  2.f});
        check(equality, "", sp1.get_uniform<GLfloat, 4>("foo_f4"), std::array{ 1.f, -1.f,  1.f, -1.f});
        check(equality, "", sp1.get_uniform<GLfloat, 4>("bar_f4"), std::array{-2.f,  2.f, -2.f,  2.f});

        check(equality, "", sp0.get_uniform<GLint>("foo_i"), 1);
        check(equality, "", sp0.get_uniform<GLint>("bar_i"), 2);
        check(equality, "", sp1.get_uniform<GLint>("foo_i"), 1);
        check(equality, "", sp1.get_uniform<GLint>("bar_i"), 2);

        check(equality, "", sp0.get_uniform<GLint, 2>("foo_i2"), std::array{ 1, -1});
        check(equality, "", sp0.get_uniform<GLint, 2>("bar_i2"), std::array{-2,  2});
        check(equality, "", sp1.get_uniform<GLint, 2>("foo_i2"), std::array{ 1, -1});
        check(equality, "", sp1.get_uniform<GLint, 2>("bar_i2"), std::array{-2,  2});

        check(equality, "", sp0.get_uniform<GLint, 3>("foo_i3"), std::array{ 1, -1,  1});
        check(equality, "", sp0.get_uniform<GLint, 3>("bar_i3"), std::array{-2,  2, -2});
        check(equality, "", sp1.get_uniform<GLint, 3>("foo_i3"), std::array{ 1, -1,  1});
        check(equality, "", sp1.get_uniform<GLint, 3>("bar_i3"), std::array{-2,  2, -2});

        check(equality, "", sp0.get_uniform<GLint, 4>("foo_i4"), std::array{ 1, -1,  1, -1});
        check(equality, "", sp0.get_uniform<GLint, 4>("bar_i4"), std::array{-2,  2, -2,  2});
        check(equality, "", sp1.get_uniform<GLint, 4>("foo_i4"), std::array{ 1, -1,  1, -1});
        check(equality, "", sp1.get_uniform<GLint, 4>("bar_i4"), std::array{-2,  2, -2,  2});

        sp0.set_uniform("foo_f", 2.0f);

        check(equality, "", sp0.get_uniform<GLfloat>("foo_f"), 2.0f);
        check(equality, "", sp0.get_uniform<GLfloat>("bar_f"), 2.0f);
        check(equality, "", sp1.get_uniform<GLfloat>("foo_f"), 1.0f);
        check(equality, "", sp1.get_uniform<GLfloat>("bar_f"), 2.0f);

        sp1.set_uniform("bar_f2", std::array{1.0f, -1.0f});

        check(equality, "", sp0.get_uniform<GLfloat, 2>("foo_f2"), std::array{ 1.f, -1.f});
        check(equality, "", sp0.get_uniform<GLfloat, 2>("bar_f2"), std::array{-2.f,  2.f});
        check(equality, "", sp1.get_uniform<GLfloat, 2>("foo_f2"), std::array{ 1.f, -1.f});
        check(equality, "", sp1.get_uniform<GLfloat, 2>("bar_f2"), std::array{ 1.f, -1.f});

        sp0.set_uniform("bar_f3", std::array{1.0f, -1.0f, 1.0f});

        check(equality, "", sp0.get_uniform<GLfloat, 3>("foo_f3"), std::array{ 1.f, -1.f,  1.f});
        check(equality, "", sp0.get_uniform<GLfloat, 3>("bar_f3"), std::array{ 1.f, -1.f,  1.f});
        check(equality, "", sp1.get_uniform<GLfloat, 3>("foo_f3"), std::array{ 1.f, -1.f,  1.f});
        check(equality, "", sp1.get_uniform<GLfloat, 3>("bar_f3"), std::array{-2.f,  2.f, -2.f});

        sp1.set_uniform("foo_f4", std::array{-2.0f, 2.0f, -2.0f, 2.0f});

        check(equality, "", sp0.get_uniform<GLfloat, 4>("foo_f4"), std::array{ 1.f, -1.f,  1.f, -1.f});
        check(equality, "", sp0.get_uniform<GLfloat, 4>("bar_f4"), std::array{-2.f,  2.f, -2.f,  2.f});
        check(equality, "", sp1.get_uniform<GLfloat, 4>("foo_f4"), std::array{-2.f,  2.f, -2.f,  2.f});
        check(equality, "", sp1.get_uniform<GLfloat, 4>("bar_f4"), std::array{-2.f,  2.f, -2.f,  2.f});

        sp0.set_uniform("bar_i", 1);

        check(equality, "", sp0.get_uniform<GLint>("foo_i"), 1);
        check(equality, "", sp0.get_uniform<GLint>("bar_i"), 1);
        check(equality, "", sp1.get_uniform<GLint>("foo_i"), 1);
        check(equality, "", sp1.get_uniform<GLint>("bar_i"), 2);

        sp1.set_uniform("foo_i2", std::array{-2, 2});

        check(equality, "", sp0.get_uniform<GLint, 2>("foo_i2"), std::array{ 1, -1});
        check(equality, "", sp0.get_uniform<GLint, 2>("bar_i2"), std::array{-2,  2});
        check(equality, "", sp1.get_uniform<GLint, 2>("foo_i2"), std::array{-2,  2});
        check(equality, "", sp1.get_uniform<GLint, 2>("bar_i2"), std::array{-2,  2});

        sp0.set_uniform("foo_i3", std::array{-2,  2, -2});

        check(equality, "", sp0.get_uniform<GLint, 3>("foo_i3"), std::array{-2,  2, -2});
        check(equality, "", sp0.get_uniform<GLint, 3>("bar_i3"), std::array{-2,  2, -2});
        check(equality, "", sp1.get_uniform<GLint, 3>("foo_i3"), std::array{ 1, -1,  1});
        check(equality, "", sp1.get_uniform<GLint, 3>("bar_i3"), std::array{-2,  2, -2});

        sp1.set_uniform("bar_i4", std::array{ 1, -1,  1, -1});

        check(equality, "", sp0.get_uniform<GLint, 4>("foo_i4"), std::array{ 1, -1,  1, -1});
        check(equality, "", sp0.get_uniform<GLint, 4>("bar_i4"), std::array{-2,  2, -2,  2});
        check(equality, "", sp1.get_uniform<GLint, 4>("foo_i4"), std::array{ 1, -1,  1, -1});
        check(equality, "", sp1.get_uniform<GLint, 4>("bar_i4"), std::array{ 1, -1,  1, -1});

    }
}
