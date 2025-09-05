////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"

#include "avocet/OpenGL/Resources/ShaderProgram.hpp"
#include "avocet/OpenGL/Geometry/Polygon.hpp"

#include "sequoia/FileSystem/FileSystem.hpp"

#include "GLFW/glfw3.h"

#include <format>
#include <iostream>
#include <source_location>

namespace {
    namespace fs = std::filesystem;

    [[nodiscard]]
    fs::path get_dir(std::string_view dirName) {
        if(const fs::path file{std::source_location::current().file_name()}; file.is_absolute()) {
            if(const auto dir{file.parent_path() / dirName}; fs::exists(dir)) {
                return dir;
            }
            else
                throw std::runtime_error{std::format("Unable to find {} directory {}", dirName, dir.generic_string())};
        }

        throw std::runtime_error{"Relative paths not supported"};
    }

    [[nodiscard]]
    fs::path get_shader_dir() { return get_dir("Shaders"); }

    [[nodiscard]]
    fs::path get_image_dir() { return get_dir("Images"); }

    [[nodiscard]]
    std::string make_label(std::string_view name, std::source_location loc = std::source_location::current()) {
        return std::format("{} created at {} line {}", name, sequoia::back(fs::path{loc.file_name()}).string(), loc.line());
    }
}

int main()
{
    try
    {
        curlew::glfw_manager manager{};
        std::cout << curlew::rendering_setup_summary(manager.get_rendering_setup());

        auto w{manager.create_window({.width{800}, .height{800}, .name{"Hello Rendering Engine"}})};

        namespace agl = avocet::opengl;
        agl::shader_program
            shaderProgram      {get_shader_dir() / "Identity.vs",           get_shader_dir() / "Monochrome.fs"},
            discShaderProgram  {get_shader_dir() / "Disc2D.vs",             get_shader_dir() / "Disc.fs"},
            shaderProgram2D    {get_shader_dir() / "IdentityTextured2D.vs", get_shader_dir() / "Textured.fs"},
            shaderProgramDouble{get_shader_dir() / "IdentityDouble.vs",     get_shader_dir() / "Monochrome.fs"};

        agl::quad<GLdouble, agl::dimensionality{3}> q{
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    sequoia::get<0>(vert) += agl::local_coordinates<GLdouble, agl::dimensionality{3}>{0.5, -0.5};
                }

                return verts;
            },
            make_label("Quad")
        };

        constexpr GLfloat radius{0.4f};
        constexpr agl::local_coordinates<GLfloat, agl::dimensionality{2}> centre{-0.5f, 0.5f};

        agl::triangle<GLfloat, agl::dimensionality{2}> disc{
            [radius, centre](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    constexpr auto scale{2 * radius / 0.5};
                    (sequoia::get<0>(vert) *= scale) += centre;
                }

                return verts;
            },
            make_label("Disc")
        };

        discShaderProgram.set_uniform("radius", radius);
        discShaderProgram.set_uniform("centre", centre.values());

        agl::polygon<GLfloat, 7, agl::dimensionality{3}> sept{
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    sequoia::get<0>(vert) += agl::local_coordinates<GLfloat, agl::dimensionality{3}>{0.5f, 0.5f};
                }

                return verts;
            },
            make_label("Septagon")
        };

        avocet::unique_image pony{get_image_dir() / "PrincessTwilightSparkle.png", avocet::flip_vertically::yes, avocet::all_channels_in_image};

        agl::polygon<GLfloat, 6, agl::dimensionality{2}, agl::texture_coordinates<GLfloat>> hex{
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    sequoia::get<0>(vert) += agl::local_coordinates<GLfloat, agl::dimensionality{2}>{-0.5f, -0.5f};
                }

                return verts;
            },
            agl::texture_2d_configurator{
                .data_view{pony},
                .decoding{agl::sampling_decoding::srgb},
                .parameter_setter{ [](){ agl::gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                .label{"Princess TS"}
            },
            make_label("Hexagon")
        };

        shaderProgram2D.set_uniform("image", 8);

        while(!glfwWindowShouldClose(&w.get())) {
            agl::gl_function{&GladGLContext::ClearColor}(ctx, 0.2f, 0.3f, 0.3f, 1.0f);
            agl::gl_function{&GladGLContext::Clear}(ctx, GL_COLOR_BUFFER_BIT);

            shaderProgramDouble.use();
            q.draw();
            discShaderProgram.use();
            disc.draw();
            shaderProgram.use();
            sept.draw();
            shaderProgram2D.use();
            hex.draw(agl::texture_unit{8});

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


