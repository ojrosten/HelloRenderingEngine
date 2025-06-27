////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"

#include "avocet/OpenGL/Graphics/ShaderProgram.hpp"
#include "avocet/OpenGL/Geometry/Polygon.hpp"

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
            shaderProgram{get_shader_dir() / "Identity.vs", get_shader_dir() / "Monochrome.fs"},
            discShaderProgram{get_shader_dir() / "Disc2D.vs", get_shader_dir() / "Disc.fs"},
            shaderProgram2D{get_shader_dir() / "Identity2D.vs", get_shader_dir() / "Monochrome.fs"},
            shaderProgramDouble{get_shader_dir() / "IdentityDouble.vs", get_shader_dir() / "Monochrome.fs"};

        agl::quad<GLdouble, agl::dimensionality{3}> q{
            [](std::ranges::random_access_range auto verts) {
                for(auto i : std::views::iota(0, std::ssize(verts))) {
                    verts[i].local_coordinates[0] += 0.5;
                    verts[i].local_coordinates[1] -= 0.5;
                }

                return verts;
            },
            make_label("Quad")
        };

        constexpr GLfloat radius{0.4f};
        constexpr std::array<GLfloat, 2> centre{-0.5, 0.5};

        agl::triangle<GLfloat, agl::dimensionality{2}> disc{
          [radius, centre](std::ranges::random_access_range auto verts) {
                for(auto i : std::views::iota(0, std::ssize(verts))) {
                    constexpr auto scale{2 * radius / 0.5};
                    for(auto&& [coord, middle] : std::views::zip(verts[i].local_coordinates.values(), centre))
                        (coord *= scale) += middle;
                }

                return verts;
            },
            make_label("Disc")
        };

        discShaderProgram.set_uniform("radius", radius);
        discShaderProgram.set_uniform("centre", centre);


        agl::polygon<GLfloat, 7, agl::dimensionality{3}> sept{
            [](std::ranges::random_access_range auto verts) {

                for(auto i : std::views::iota(0, std::ssize(verts))) {
                    verts[i].local_coordinates[0] += 0.5;
                    verts[i].local_coordinates[1] += 0.5;
                }

                return verts;
            },
            make_label("Septagon")
        };

        agl::polygon<GLfloat, 6, agl::dimensionality{2}> hex{
            [](std::ranges::random_access_range auto verts) {
                for(auto i : std::views::iota(0, std::ssize(verts))) {
                    verts[i].local_coordinates[0] -= 0.5;
                    verts[i].local_coordinates[1] -= 0.5;
                }

                return verts;
            },
            make_label("Hexagon")
        };

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shaderProgramDouble.use();
            q.draw();
            discShaderProgram.use();
            disc.draw();
            shaderProgram.use();
            sept.draw();
            shaderProgram2D.use();
            hex.draw();

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


