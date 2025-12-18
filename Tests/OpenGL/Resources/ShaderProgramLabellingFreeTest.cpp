////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramLabellingFreeTest.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path shader_program_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_labelling_free_test::labelling_tests(const curlew::opengl_window& w)
    {
        namespace agl = avocet::opengl;
        const auto shaderDir{working_materials()};

        agl::shader_program sp{
            w.context(),
            shaderDir / "Identity.vs",
            shaderDir / "Monochrome.fs"
        };

        check(equivalence, "", sp.extract_label(), "Identity.vs / Monochrome.fs");
    }
}
