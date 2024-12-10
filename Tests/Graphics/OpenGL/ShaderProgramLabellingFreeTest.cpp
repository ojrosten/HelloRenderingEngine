////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramLabellingFreeTest.hpp"
#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path shader_program_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_labelling_free_test::labelling_tests()
    {
        namespace agl = avocet::opengl;
        const auto shaderDir{working_materials()};

        agl::shader_program sp{
            shaderDir / "Identity.vs",
            shaderDir / "Monochrome.fs"
        };

        check_object_label("Shader Program Label", agl::object_identifier::program, sp.resource().handle(), "Identity.vs / Monochrome.fs");
    }
}
