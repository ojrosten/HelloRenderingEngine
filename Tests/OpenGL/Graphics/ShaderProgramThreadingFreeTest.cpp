////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramThreadingFreeTest.hpp"
#include "avocet/OpenGL/Graphics/ShaderProgram.hpp"

#include <future>
#include <memory>
#include <thread>

namespace avocet::testing
{
    namespace agl = avocet::opengl;
    namespace fs = std::filesystem;

    namespace
    {
        GLint get_program_index() {
            GLint param{};
            agl::gl_function{glGetIntegerv}(GL_CURRENT_PROGRAM, &param);
            return param;
        }

        GLint make_and_use_shader_program(curlew::glfw_manager& manager, const fs::path& shaderDir) {
            auto w{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
            agl::shader_program sp{shaderDir / "Identity.vs", shaderDir / "Monochrome.fs"};
            sp.use();
            return get_program_index();
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
        curlew::glfw_manager manager{};

        check_serial_tracking(manager);
        check_threaded_tracking(manager);
    }

    void shader_program_threading_free_test::check_serial_tracking(curlew::glfw_manager& manager)
    {
        const auto shaderDir{working_materials()};
        const auto
            prog0{make_and_use_shader_program(manager, shaderDir)},
            prog1{make_and_use_shader_program(manager, shaderDir)};

        if(check("Bounded Context", (!prog1) || (prog0 == prog1)))
        {
            check("prog0 should report > 0", prog0 > 0);
            check("prog1 should report > 0", prog1 > 0);
        }
    }


    void shader_program_threading_free_test::check_threaded_tracking(curlew::glfw_manager& manager)
    {
        const auto shaderDir{working_materials()};
        std::unique_ptr<agl::shader_program> spPtr{};
        std::packaged_task<GLint(void)>  task0{
            [&]() {
                auto w{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
                spPtr = std::make_unique<agl::shader_program>(shaderDir / "Identity.vs", shaderDir / "Monochrome.fs");
                spPtr->use();
                return get_program_index();
            }
        };

        auto future0{task0.get_future()};

        std::packaged_task<GLint(void)> task1{
            [&]() { return make_and_use_shader_program(manager, shaderDir); }
        };

        auto future1{task1.get_future()};

        std::jthread worker0{std::move(task0)}, worker1{std::move(task1)};

        const auto prog0{future0.get()}, prog1{future1.get()};
        if(check("Bounded Context", (!prog1) || (prog0 == prog1)))
        {
            check("prog0 should report > 0", prog0 > 0);
            check("prog1 should report > 0", prog1 > 0);
        }
    }
}
