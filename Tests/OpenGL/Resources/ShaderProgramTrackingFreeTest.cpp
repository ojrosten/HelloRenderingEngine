////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramTrackingFreeTest.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path shader_program_tracking_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_tracking_free_test::run_tests()
    {
        auto creator{
            [shaderDir{working_materials()}](const agl::resourceful_context& ctx) {
                return agl::shader_program{ctx, shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
            }
        };

        auto utilizer{
            [](const agl::shader_program& sp) {
                sp.use();
            }
        };

        execute_tests("UseProgram", agl::int_names::current_program, creator, utilizer);
    }
}
