////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramFileExistenceFreeTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path shader_program_file_existence_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_file_existence_free_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        namespace agl = avocet::opengl;
        const auto shaderDir{working_materials()};

        check_exception_thrown<std::runtime_error>(
            "Missing Vertex Shader",
            [&shaderDir](){
                agl::shader_program sp{
                    shaderDir,
                    shaderDir / "Monochrome.fs"
                };
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Misnamed Vertex Shader",
            [&shaderDir](){
                agl::shader_program sp{
                    shaderDir / "foo.vs",
                    shaderDir / "Monochrome.fs"
                };
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Missing Fragment Shader",
            [&shaderDir](){
                agl::shader_program sp{
                    shaderDir / "Identity.vs",
                    shaderDir
                };
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Misnamed Fragment Shader",
            [&shaderDir](){
                agl::shader_program sp{
                    shaderDir / "Identity.vs",
                    shaderDir / "bar.fs"
                };
            }
        );
    }
}
