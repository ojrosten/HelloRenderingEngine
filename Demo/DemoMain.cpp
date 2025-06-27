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
    fs::path get_image_dir() {
        if(const fs::path file{std::source_location::current().file_name()}; file.is_absolute()) {
            if(const auto dir{file.parent_path() / "Images"}; fs::exists(dir)) {
                return dir;
            }
            else
                throw std::runtime_error{std::format("Unable to find images directory {}", dir.generic_string())};
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
            shaderProgram2D{get_shader_dir() / "IdentityTextured2D.vs", get_shader_dir() / "Textured.fs"};

        const avocet::unique_image princess{get_image_dir() / "PrincessTwilightSparkle.png", avocet::flip_vertically::yes, avocet::all_channels_in_image};
        agl::polygon<GLfloat, 4, agl::dimensionality{2}, agl::texture_coordinates<GLfloat>> q{
            [](std::ranges::random_access_range auto verts) {
                for(auto i : std::views::iota(0, std::ssize(verts))) {
                    verts[i].local_coordinates[0] *= 2.0;
                    verts[i].local_coordinates[1] *= 2.0;
                    (std::get<0>(verts[i].additional_attributes)[0] *= std::sqrt(2.0f)) += (1.0f - std::sqrt(2.0f))/2.0f;
                    (std::get<0>(verts[i].additional_attributes)[1] *= std::sqrt(2.0f)) += (1.0f - std::sqrt(2.0f))/2.0f;
                }

                return verts;
            },
            agl::texture_2d_configurator{.data_view{princess}, .decoding{agl::sampling_decoding::srgb}, .parameter_setter{agl::default_texture_2d_parameter_setter{}}, .label{make_label("Pony")}},
            make_label("Quad")
        };

        shaderProgram2D.set_uniform("image", 0);

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shaderProgram2D.use();
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


