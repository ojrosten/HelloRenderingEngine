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

#include <iostream>
#include <format>
#include <print>

namespace agl = avocet::opengl;

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
    fs::path get_vertex_shader_dir() { return get_shader_dir() / "Vertex"; }

    [[nodiscard]]
    fs::path get_fragment_shader_dir() { return get_shader_dir() / "Fragment"; }

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
        const auto renderingSetup{manager.get_rendering_setup()};
        std::cout << curlew::rendering_setup_summary(renderingSetup);

        const std::vector<agl::message_id> acceptableWarnings{
            [&renderingSetup]() -> std::vector<agl::message_id> {
                if(curlew::is_intel(renderingSetup.renderer))
                    return {agl::message_id{2}};
                else if(curlew::is_nvidia(renderingSetup.renderer))
                    return {agl::message_id{131204}, agl::message_id{131218}};

                return {};
            }()
        };

        auto w{
            manager.create_window(
                {.width {800},
                 .height{800},
                 .name{"Hello Rendering Engine"},
                 .hiding{curlew::window_hiding_mode::off},
                 .debug_mode{agl::debugging_mode::dynamic},
                 .prologue{},
                 .epilogue{agl::standard_error_checker{agl::num_messages{10}, agl::default_debug_info_processor{acceptableWarnings}}},
                 .samples{4}
                }
            )
        };
        const auto& ctx{w.context()};

        agl::shader_program
            discShaderProgram2D             {ctx, get_vertex_shader_dir() / "2D" / "Disc.vs",                  get_fragment_shader_dir() / "2D"      / "Disc.fs"},
            discShaderProgram2DTextured     {ctx, get_vertex_shader_dir() / "2D" / "DiscTextured.vs",          get_fragment_shader_dir() / "2D"      / "DiscTextured.fs"},
            shaderProgram2DTextured         {ctx, get_vertex_shader_dir() / "2D" / "IdentityTextured.vs",      get_fragment_shader_dir() / "General" / "Textured.fs"},
            shaderProgram2DMixedTextures    {ctx, get_vertex_shader_dir() / "2D" / "IdentityTwiceTextured.vs", get_fragment_shader_dir() / "General" / "MixedTextures.fs"},
            shaderProgram3DDoubleMonochrome {ctx, get_vertex_shader_dir() / "3D" / "IdentityDouble.vs",        get_fragment_shader_dir() / "General" / "Monochrome.fs"};

        avocet::unique_image twilight  {get_image_dir() / "PrincessTwilightSparkle.png", avocet::flip_vertically::yes, avocet::all_channels_in_image};
        avocet::unique_image fluttershy{get_image_dir() / "Fluttershy.png",              avocet::flip_vertically::yes, avocet::all_channels_in_image};

        agl::quad<GLdouble, agl::dimensionality{3}> partiallyTransparentQuadUpper{
            ctx,
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    (sequoia::get<0>(vert) *= 0.75) += agl::local_coordinates<GLdouble, agl::dimensionality{3}> {-0.15, -0.2};
                }

                return verts;
            },
            make_label("Quad")
        };

        agl::quad<GLdouble, agl::dimensionality{3}> partiallyTransparentQuadLower{
            ctx,
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    (sequoia::get<0>(vert) *= 0.75) += agl::local_coordinates<GLdouble, agl::dimensionality{3}>{-0.15, -0.75};
                }

                return verts;
            },
            make_label("Quad")
        };


        shaderProgram3DDoubleMonochrome.set_uniform("colour", std::array{1.0f, 0.5f, 0.2f, 0.4f});


        constexpr GLfloat radius{0.4f};
        constexpr agl::local_coordinates<GLfloat, agl::dimensionality{2}> centre{-0.5f, 0.5f};

        agl::triangle<GLfloat, agl::dimensionality{2}, agl::texture_coordinates<GLfloat>> disc{
            ctx,
            [radius, centre](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    constexpr auto scale{2 * radius / 0.5};
                    (sequoia::get<0>(vert) *= scale) += centre;
                    (sequoia::get<1>(vert) *= 1.6f)  += agl::texture_coordinates<GLfloat>{-0.3f, -0.3f};
                }

                return verts;
            },
            agl::texture_2d_configurator{
                .data_view{fluttershy},
                .decoding{agl::sampling_decoding::srgb},
                .parameter_setter{ [&ctx]() { agl::gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                .label{"Fluttershy"}
            },
            make_label("Disc")
        };

        discShaderProgram2DTextured.set_uniform("radius", radius);
        discShaderProgram2DTextured.set_uniform("centre", centre.values());
        discShaderProgram2DTextured.set_uniform("image", 5);

        agl::polygon<GLfloat, 7, agl::dimensionality{2}, agl::texture_coordinates<GLfloat>, agl::texture_coordinates<GLfloat>> sept{
            ctx,
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    sequoia::get<0>(vert) += agl::local_coordinates<GLfloat, agl::dimensionality{2}>{0.5f, 0.5f};
                }

                return verts;
            },
            std::array{
                agl::texture_2d_configurator{
                    .data_view{twilight},
                    .decoding{agl::sampling_decoding::srgb},
                    .parameter_setter{ [&ctx]() { agl::gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                    .label{"Twilight"}
                },
                agl::texture_2d_configurator{
                    .data_view{fluttershy},
                    .decoding{agl::sampling_decoding::srgb},
                    .parameter_setter{ [&ctx]() { agl::gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                    .label{"Fluttershy"}
                }
            },
            make_label("Septagon")
        };

        shaderProgram2DMixedTextures.set_uniform("image0", 2);
        shaderProgram2DMixedTextures.set_uniform("image1", 3);

        agl::polygon<GLfloat, 6, agl::dimensionality{2}, agl::texture_coordinates<GLfloat>> hex{
            ctx,
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    sequoia::get<0>(vert) += agl::local_coordinates<GLfloat, agl::dimensionality{2}>{-0.5f, -0.5f};
                }

                return verts;
            },
            agl::texture_2d_configurator{
                .data_view{twilight},
                .decoding{agl::sampling_decoding::srgb},
                .parameter_setter{ [&ctx](){ agl::gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                .label{"Princess TS"}
            },
            make_label("Hexagon")
        };

        shaderProgram2DTextured.set_uniform("image", 8);

        avocet::unique_image hearty{get_image_dir() / "Hearts.png", avocet::flip_vertically::yes, avocet::all_channels_in_image};

        agl::quad<GLfloat, agl::dimensionality{2}, agl::texture_coordinates<GLfloat >> hearts{
            ctx,
            [](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    (sequoia::get<0>(vert) *= 1.4f) += agl::local_coordinates<GLfloat, agl::dimensionality{2}>{-0.5f, -0.5f};
                }

                return verts;
            },
            agl::texture_2d_configurator{
                .data_view{hearty},
                .decoding{agl::sampling_decoding::srgb},
                .parameter_setter{ [&ctx]() { agl::gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                .label{"Hearts"}
            },
            make_label("Wall of Hearts")
        };

        constexpr GLfloat cutoutRadius{0.25f};
        constexpr agl::local_coordinates<GLfloat, agl::dimensionality{2}> cutoutCentre{-0.5f, -0.5f};

        agl::triangle<GLfloat, agl::dimensionality{2}> cutout{
            ctx,
            [cutoutRadius, cutoutCentre](std::ranges::random_access_range auto verts) {
                for(auto& vert : verts) {
                    constexpr auto scale{2.0 * cutoutRadius / 0.5};
                    (sequoia::get<0>(vert) *= scale) += cutoutCentre;
                }

                return verts;
            },
            make_label("Cutout")
        };

        discShaderProgram2D.set_uniform("radius", cutoutRadius);
        discShaderProgram2D.set_uniform("centre", cutoutCentre.values());

        while(!glfwWindowShouldClose(&w.get())) {
            agl::gl_function{&GladGLContext::ClearColor}(ctx, 0.2f, 0.3f, 0.3f, 1.0f);

            agl::gl_function{&GladGLContext::Clear}(ctx, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            discShaderProgram2DTextured.use();
            disc.draw(agl::texture_unit{5});

            shaderProgram2DMixedTextures.use();
            sept.draw(std::array{agl::texture_unit{2}, agl::texture_unit{3}});

            agl::gl_function{&GladGLContext::Enable}(ctx, GL_STENCIL_TEST);

            agl::gl_function{&GladGLContext::StencilFunc}(ctx, GL_GREATER, 1, 255);
            agl::gl_function{&GladGLContext::StencilOp}(ctx, GL_KEEP, GL_KEEP, GL_REPLACE);
            discShaderProgram2D.use();
            cutout.draw();

            agl::gl_function{&GladGLContext::StencilOp}(ctx, GL_KEEP, GL_KEEP, GL_KEEP);
            shaderProgram2DTextured.use();
            hearts.draw(agl::texture_unit{8});

            agl::gl_function{&GladGLContext::StencilFunc}(ctx, GL_EQUAL, 1, 255);
            shaderProgram2DTextured.use();
            hex.draw(agl::texture_unit{8});

            agl::gl_function{&GladGLContext::Disable}(ctx, GL_STENCIL_TEST);

            shaderProgram3DDoubleMonochrome.use();
            partiallyTransparentQuadUpper.draw();

            agl::gl_function{&GladGLContext::Enable}(ctx, GL_BLEND);
            agl::gl_function{&GladGLContext::BlendFunc}(ctx, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            partiallyTransparentQuadLower.draw();

            agl::gl_function{&GladGLContext::Disable}(ctx, GL_BLEND);

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


