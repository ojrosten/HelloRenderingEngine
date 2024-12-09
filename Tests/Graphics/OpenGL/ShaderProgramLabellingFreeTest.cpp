////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramLabellingFreeTest.hpp"
#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"
#include "avocet/Utilities/OpenGL/Casts.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/Graphics/OpenGL/GLFunction.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path shader_program_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_labelling_free_test::run_tests()
    {
        if constexpr(!avocet::has_ndebug())
            labelling_tests();
    }

    void shader_program_labelling_free_test::labelling_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
        if(avocet::opengl::object_labels_activated()) {
            namespace agl = avocet::opengl;
            const auto shaderDir{working_materials()};

            agl::shader_program sp{
                shaderDir / "Identity.vs",
                shaderDir / "Monochrome.fs"
            };

            check_object_label("Shader Program Label", sp, agl::object_identifier::program, "Identity.vs / Monochrome.fs");
        }
    }
}
