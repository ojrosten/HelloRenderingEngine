////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramThreadingFreeTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Graphics/ShaderProgram.hpp"

#include <memory>
#include <thread>

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    namespace
    {
        GLint get_program_index() {
            GLint param{};
            agl::gl_function{glGetIntegerv}(GL_CURRENT_PROGRAM, &param);
            return param;
        }
    }

    [[nodiscard]]
    std::filesystem::path shader_program_threading_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_threading_free_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        const auto shaderDir{working_materials()};

        std::unique_ptr<agl::shader_program> spPtr{};

        {
            std::jthread worker{
                [&, this]() {
                    auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
                    spPtr = std::make_unique<agl::shader_program>(shaderDir / "Identity.vs", shaderDir / "Monochrome.fs");
                    spPtr->use();
                    check("", get_program_index() > 0);
                }
            };
        }

        {
            std::jthread worker{
                [&, this]() {
                    auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
                    agl::shader_program sp{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
                    sp.use();
                    check("", get_program_index() > 0);
                }
            };
        }

        {
            std::jthread worker{
                [&, this]() {
                    {
                        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
                        agl::shader_program sp{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
                        sp.use();
                        check("", get_program_index() > 0);
                    }
                    {
                        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
                        agl::shader_program sp{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
                        sp.use();
                        check("", get_program_index() > 0);
                    }
                }
            };
        }
    }
}
