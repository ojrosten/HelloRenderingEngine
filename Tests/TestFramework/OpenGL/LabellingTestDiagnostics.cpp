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

        check(equivalence, "Label is too short", sp.extract_label(), "Identity.vs / Monochrome.fss");
        check(equivalence, "Label is too long",  sp.extract_label(), "Identity.vs / Monochrome.f");
        check(equivalence, "Label has a typo",   sp.extract_label(), "Identity.vs / Monocjrome.fs");
    }
}
