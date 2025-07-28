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
        agl::resource_handle get_program_index() {
            GLint param{};
            agl::gl_function{glGetIntegerv}(GL_CURRENT_PROGRAM, &param);
            if(param < 0)
                throw std::runtime_error{"Negative program index!"};

            return agl::resource_handle{static_cast<GLuint>(param)};
        }

        agl::resource_handle make_and_use_shader_program(curlew::glfw_manager& manager, const fs::path& shaderDir) {
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

        check_program_indices(prog0, prog1);
    }


    void shader_program_threading_free_test::check_threaded_tracking(curlew::glfw_manager& manager)
    {
        const auto shaderDir{working_materials()};
        std::unique_ptr<agl::shader_program> spPtr{};
        std::packaged_task<agl::resource_handle(void)>  task0{
            [&]() {
                auto w{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
                spPtr = std::make_unique<agl::shader_program>(shaderDir / "Identity.vs", shaderDir / "Monochrome.fs");
                spPtr->use();
                return get_program_index();
            }
        };

        auto future0{task0.get_future()};
        std::jthread worker0{std::move(task0)};
        const auto prog0{future0.get()};

        std::packaged_task<agl::resource_handle(void)> task1{
            [&]() { return make_and_use_shader_program(manager, shaderDir); }
        };

        auto future1{task1.get_future()};
        std::jthread worker1{std::move(task1)};
        const auto prog1{future1.get()};

        check_program_indices(prog0, prog1);
    }

    void shader_program_threading_free_test::check_program_indices(const avocet::opengl::resource_handle& prog0, const avocet::opengl::resource_handle& prog1, const std::source_location& loc)
    {
        if(check(reporter{"Bounded Context: Either program 1 should have failed to be utilized or programs 0 and 1 have the same index", loc}, (!prog1) || (prog0 == prog1)))
        {
            check(reporter{"prog0 should report > 0", loc}, prog0.index() > 0);
            check(reporter{"prog1 should report > 0", loc}, prog1.index() > 0);
        }
    }
}
