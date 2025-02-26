////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"
#include "avocet/Geometry/OpenGL/PrimitiveGeometry.hpp"

#include "sequoia/FileSystem/FileSystem.hpp"

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

    [[nodiscard]]
    std::string make_label(std::string_view name, std::source_location loc = std::source_location::current()) {
        return std::format("{} created at {} line {}", name, sequoia::back(fs::path{loc.file_name()}).string(), loc.line());
    }
}

int main()
{
    try
    {
        std::cout << curlew::rendering_setup_summary();

        curlew::glfw_manager manager{};
        auto w{manager.create_window({.width{1500}, .height{1200}, .name{"Hello Rendering Engine"}})};

        namespace agl = avocet::opengl;
        agl::shader_program shaderProgram{get_shader_dir() / "Mandelbrot.vs", get_shader_dir() / "Mandelbrot.fs"};
        shaderProgram.use();

        const auto progIndex{shaderProgram.resource().handle().index()};
        const auto xRangeLoc{agl::gl_function{glGetUniformLocation}(progIndex, "xRange")};
        const auto yRangeLoc{agl::gl_function{glGetUniformLocation}(progIndex, "yRange")};
        const auto xOffsetLoc{agl::gl_function{glGetUniformLocation}(progIndex, "xOffset")};
        const auto yOffsetLoc{agl::gl_function{glGetUniformLocation}(progIndex, "yOffset")};

        agl::gl_function{glUniform1f}(xRangeLoc, 0.025f);
        agl::gl_function{glUniform1f}(yRangeLoc, 0.02f);
        agl::gl_function{glUniform1f}(xOffsetLoc, -1.4f);
        agl::gl_function{glUniform1f}(yOffsetLoc, 0);

        agl::quad q{make_label("Quad")};

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shaderProgram.use();
            q.draw();

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


