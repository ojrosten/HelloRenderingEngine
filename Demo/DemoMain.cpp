////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"
#include "avocet/Geometry/OpenGL/Polygon.hpp"

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
        auto w{manager.create_window({.width{800}, .height{800}, .name{"Hello Rendering Engine"}})};

        namespace agl = avocet::opengl;

        agl::shader_program
            discShaderProgram{get_shader_dir() / "Disc.vs", get_shader_dir() / "Disc.fs"};

        constexpr std::array<GLfloat, 2> centre{0.2f, 0.3f};
        constexpr GLfloat scale{2.0};

        agl::triangle<GLfloat, agl::dimensionality{3}> tri{
            [&centre, &scale](std::ranges::random_access_range auto verts) {
                for(auto i : std::views::iota(0, std::ssize(verts))) {
                    if(!(i % 3))       (verts[i] *= scale) += centre[0];
                    if(!((i - 1) % 3)) (verts[i] *= scale) += centre[1];
                }

                return verts;
            },
            make_label("Disc")
        };

        discShaderProgram.use();

        const auto centreLoc{agl::gl_function{glGetUniformLocation}(discShaderProgram.resource().handle().index(), "centre")};
        const auto radiusLoc{agl::gl_function{glGetUniformLocation}(discShaderProgram.resource().handle().index(), "radius")};
        agl::gl_function{glUniform2f}(centreLoc, centre[0], centre[1]);
        agl::gl_function{glUniform1f}(radiusLoc, 0.25*scale);

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            discShaderProgram.use();
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


