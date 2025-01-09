////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "LabellingTestDiagnostics.hpp"
#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path labelling_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void labelling_false_negative_test::labelling_tests()
    {
        namespace agl = avocet::opengl;
        const auto shaderDir{working_materials()};

        agl::shader_program sp{
            shaderDir / "Identity.vs",
            shaderDir / "Monochrome.fs"
        };

        check_object_label("Shader Program Label typo",      agl::object_identifier::program, sp.resource().handle(), "Identity.vs / Monochrome.ff");
        check_object_label("Shader Program Label too long",  agl::object_identifier::program, sp.resource().handle(), "Identity.vs / Monochrome.f");
        check_object_label("Shader Program Label too short", agl::object_identifier::program, sp.resource().handle(), "Identity.vs / Monochrome.fss");
    }

    [[nodiscard]]
    std::filesystem::path labelling_false_positive_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void labelling_false_positive_test::labelling_tests()
    {
        namespace agl = avocet::opengl;
        const auto shaderDir{working_materials()};

        agl::shader_program sp{
            shaderDir / "Identity.vs",
            shaderDir / "Monochrome.fs"
        };

        check_exception_thrown<std::runtime_error>(
            "Incorrect object identifier",
            [&sp, this](){ check_object_label("Shader Program Label", agl::object_identifier::framebuffer, sp.resource().handle(), "Identity.vs / Monochrome.fs"); }
        );

        check_exception_thrown<std::runtime_error>(
            "Incorrect handle",
            [&sp, this](){ check_object_label("Shader Program Label", agl::object_identifier::program, agl::resource_handle{42}, "Identity.vs / Monochrome.fs"); }
        );
    }
}
