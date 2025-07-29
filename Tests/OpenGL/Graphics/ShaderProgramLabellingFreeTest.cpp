////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramLabellingFreeTest.hpp"
#include "avocet/OpenGL/Graphics/ShaderProgram.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path shader_program_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_labelling_free_test::labelling_tests(curlew::window& w)
    {
        namespace agl = avocet::opengl;
        const auto shaderDir{working_materials()};

        agl::shader_program sp{
            shaderDir / "Identity.vs",
            shaderDir / "Monochrome.fs",
            w.context_index()
        };

        check(equivalence, "", sp.extract_label(), "Identity.vs / Monochrome.fs");
    }
}
