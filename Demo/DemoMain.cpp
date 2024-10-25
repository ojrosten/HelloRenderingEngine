////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"
#include "avocet/Geometry/OpenGL/Triangle.hpp"

#include "GLFW/glfw3.h"

#include <format>
#include <iostream>
#include <source_location>

namespace {
    namespace fs = std::filesystem;

    [[nodiscard]]
    fs::path get_shader_dir() {
        if(const fs::path file{std::source_location::current().file_name()}; file.is_absolute()) {
            if(const auto dir{file.parent_path() / "Shaders"}; fs::exists(dir)) {
                return dir;
            }
            else
                throw std::runtime_error{std::format("Unable to find shader directory {}", dir.generic_string())};
        }

        throw std::runtime_error{"Relative paths not supported"};
    }
}

int main()
{
    try
    {
        curlew::glfw_manager manager{};
        auto w{manager.create_window({.name{"Hello Rendering Engine"}})};

        namespace agl = avocet::opengl;

        agl::shader_program shaderProgram{get_shader_dir() / "Identity.vs", get_shader_dir() / "Monochrome.fs"};
        agl::triangle tri{};

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shaderProgram.use();
            tri.draw();

            glfwSwapBuffers(&w.get());
            glfwPollEvents();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what();
    }
    catch(...)
    {
        std::cerr << "Unrecognized error\n";
    }
}


