////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "PonyPolygons.hpp"

#include "sequoia/FileSystem/FileSystem.hpp"

namespace avocet::opengl::testing {

    namespace {
        namespace fs = std::filesystem;

        [[nodiscard]]
        fs::path get_dir(std::string_view dirName) {
            if(const fs::path file{std::source_location::current().file_name()}; file.is_absolute()) {
                if(const auto dir{file.parent_path().parent_path() / dirName}; fs::exists(dir)) {
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

        using payload_type = capable_context::payload_type;

        template<class... Caps>
        [[nodiscard]]
        payload_type make_payload(const Caps&... caps) {
            payload_type payload{};
            ((std::get<std::optional<Caps>>(payload) = caps), ...);
            return payload;
        }

        template<class... Caps>
        void set_payload(const capable_context& ctx, const Caps&... targetCaps) {
            ctx.new_payload(make_payload(targetCaps...));
        }

        constexpr GLfloat discRadius{0.4f},
                          cutoutRadius{0.25f};
        constexpr local_coordinates<GLfloat, dimensionality{2}>
            discCentre{-0.5f, 0.5f},
            cutoutCentre{-0.5f, -0.5f};

        [[nodiscard]]
        triangle<GLfloat, dimensionality{2}> make_cutout(const activating_context& ctx) {
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        constexpr GLfloat scale{4.0f * cutoutRadius};
                        (sequoia::get<0>(vert) *= scale) += cutoutCentre;
                    }
                
                    return verts;
                },
                make_label("Cutout")
            };
        }

        [[nodiscard]]
        triangle<GLfloat, dimensionality{2}, texture_coordinates<GLfloat>> make_disc(const activating_context& ctx, image_view fluttershy) {
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        constexpr GLfloat scale{4.0f * discRadius};
                        (sequoia::get<0>(vert) *= scale) += discCentre;
                        (sequoia::get<1>(vert) *= 1.6f)  += texture_coordinates<GLfloat>{-0.3f, -0.3f};
                    }
                
                    return verts;
                },
                texture_2d_configurator{
                    .data_view{fluttershy},
                    .decoding{sampling_decoding::srgb},
                    .parameter_setter{ [&ctx]() { gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                    .label{"Fluttershy"}
                },
                make_label("Disc")
            };
        }

        [[nodiscard]]
        quad<GLfloat, dimensionality{2}, texture_coordinates<GLfloat>> make_lower_hearts(const activating_context& ctx, image_view hearty){
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        (sequoia::get<0>(vert) *= 1.4f) += local_coordinates<GLfloat, dimensionality{2}>{-0.5f, -0.5f};
                    }
                
                    return verts;
                },
                texture_2d_configurator{
                    .data_view{hearty},
                    .decoding{sampling_decoding::srgb},
                    .parameter_setter{ [&ctx]() { gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                    .label{"Hearts"}
                },
                make_label("Wall of Hearts")
            };
        }

        [[nodiscard]]
        quad<GLfloat, dimensionality{3}, texture_coordinates<GLfloat>> make_upper_hearts(const activating_context& ctx, image_view hearty) {
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        (sequoia::get<0>(vert) *= 1.4f) += local_coordinates<GLfloat, dimensionality{3}>{0.5f, 0.5f, 0.1f};
                    }
                
                    return verts;
                },
                texture_2d_configurator{
                    .data_view{hearty},
                    .decoding{sampling_decoding::srgb},
                    .parameter_setter{ [&ctx]() { gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                    .label{"Hearts"}
                },
                make_label("Upper Wall of Hearts")
            };
        }

        [[nodiscard]]
        quad<GLdouble, dimensionality{3}> make_upper_quad(const activating_context& ctx) {
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        (sequoia::get<0>(vert) *= 0.75) += local_coordinates<GLdouble, dimensionality{3}> {-0.15, -0.2};
                    }
                
                    return verts;
                },
                make_label("Upper Quad")
            };
        }

        [[nodiscard]]
        quad<GLdouble, dimensionality{3}> make_lower_quad(const activating_context& ctx){
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        (sequoia::get<0>(vert) *= 0.75) += local_coordinates<GLdouble, dimensionality{3}>{-0.15, -0.75};
                    }
                
                    return verts;
                },
                make_label("Lower Quad")
            };
        }

        [[nodiscard]]
        polygon<GLfloat, 6, dimensionality{2}, texture_coordinates<GLfloat>> make_hexagon(const activating_context& ctx, image_view twilight) {
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        sequoia::get<0>(vert) += local_coordinates<GLfloat, dimensionality{2}>{-0.5f, -0.5f};
                    }
                
                    return verts;
                },
                texture_2d_configurator{
                    .data_view{twilight},
                    .decoding{sampling_decoding::srgb},
                    .parameter_setter{ [&ctx](){ gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                    .label{"Princess TS"}
                },
                make_label("Hexagon")
            };
        }

        [[nodiscard]]
        polygon<GLfloat, 7, dimensionality{2}, texture_coordinates<GLfloat>, texture_coordinates<GLfloat>> make_septagon(const activating_context& ctx, image_view twilight, image_view fluttershy){
            return {
                ctx,
                [](std::ranges::random_access_range auto verts) {
                    for(auto& vert : verts) {
                        sequoia::get<0>(vert) += local_coordinates<GLfloat, dimensionality{2}>{0.5f, 0.5f};
                    }
                
                    return verts;
                },
                std::array{
                    texture_2d_configurator{
                        .data_view{twilight},
                        .decoding{sampling_decoding::srgb},
                        .parameter_setter{ [&ctx]() { gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                        .label{"Twilight"}
                    },
                    texture_2d_configurator{
                        .data_view{fluttershy},
                        .decoding{sampling_decoding::srgb},
                        .parameter_setter{ [&ctx]() { gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); }},
                        .label{"Fluttershy"}
                    }
                },
                make_label("Septagon")
            };
        }
    }

    pony_polygons::pony_polygons(const capable_context& ctx)
        : m_Context{ctx}
        , m_DiscShaderProgram2D             {ctx, get_vertex_shader_dir() / "2D" / "Disc.vs",                  get_fragment_shader_dir() / "2D"      / "Disc.fs"}
        , m_DiscShaderProgram2DTextured     {ctx, get_vertex_shader_dir() / "2D" / "DiscTextured.vs",          get_fragment_shader_dir() / "2D"      / "DiscTextured.fs"}
        , m_ShaderProgram2DTextured         {ctx, get_vertex_shader_dir() / "2D" / "IdentityTextured.vs",      get_fragment_shader_dir() / "General" / "Textured.fs"}
        , m_ShaderProgram2DMonochrome       {ctx, get_vertex_shader_dir() / "2D" / "Identity.vs",              get_fragment_shader_dir() / "General" / "Monochrome.fs"}
        , m_ShaderProgram2DMixedTextures    {ctx, get_vertex_shader_dir() / "2D" / "IdentityTwiceTextured.vs", get_fragment_shader_dir() / "General" / "MixedTextures.fs"}
        , m_ShaderProgram3DTextured         {ctx, get_vertex_shader_dir() / "3D" / "IdentityTextured.vs",      get_fragment_shader_dir() / "General" / "Textured.fs"}
        , m_ShaderProgram3DDoubleMonochrome {ctx, get_vertex_shader_dir() / "3D" / "IdentityDouble.vs",        get_fragment_shader_dir() / "General" / "Monochrome.fs"}
        , m_Twilight  {get_image_dir() / "PrincessTwilightSparkle.png", avocet::flip_vertically::yes, avocet::all_channels_in_image}
        , m_Fluttershy{get_image_dir() / "Fluttershy.png",              avocet::flip_vertically::yes, avocet::all_channels_in_image}
        , m_Hearty    {get_image_dir() / "Hearts.png", avocet::flip_vertically::yes, avocet::all_channels_in_image}
        , m_Cutout                       {make_cutout      (ctx                           )}
        , m_Disc                         {make_disc        (ctx, m_Fluttershy             )}
        , m_LowerHearts                  {make_lower_hearts(ctx, m_Hearty                 )}
        , m_UpperHearts                  {make_upper_hearts(ctx, m_Hearty                 )}
        , m_PartiallyTransparentQuadUpper{make_upper_quad  (ctx                           )}
        , m_PartiallyTransparentQuadLower{make_lower_quad  (ctx                           )}
        , m_Hexagon                      {make_hexagon     (ctx,  m_Twilight              )}
        , m_Septagon                     {make_septagon    (ctx,  m_Twilight, m_Fluttershy)}
    {
        m_ShaderProgram3DDoubleMonochrome.set_uniform("colour", std::array{1.0f, 0.5f, 0.2f, 0.4f});
        m_DiscShaderProgram2DTextured.set_uniform("radius", discRadius);
        m_DiscShaderProgram2DTextured.set_uniform("centre", discCentre.values());
        m_DiscShaderProgram2DTextured.set_uniform("image", 5);
        m_ShaderProgram2DMixedTextures.set_uniform("image0", 2);
        m_ShaderProgram2DMixedTextures.set_uniform("image1", 3);
        m_ShaderProgram2DTextured.set_uniform("image", 8);
        m_ShaderProgram3DTextured.set_uniform("image", 7);
        m_DiscShaderProgram2D.set_uniform("radius", cutoutRadius);
        m_DiscShaderProgram2D.set_uniform("centre", cutoutCentre.values());
    }

    void pony_polygons::draw() {
        {
            set_payload(m_Context);
            m_DiscShaderProgram2DTextured.use();
            m_Disc.draw(texture_unit{5});
        }

        {
            set_payload(m_Context, capabilities::gl_depth_test{});
            m_ShaderProgram2DMixedTextures.use();
            m_Septagon.draw(std::array{texture_unit{2}, texture_unit{3}});
        }

        {
            gl_function{&GladGLContext::PolygonMode}(m_Context, GL_FRONT_AND_BACK, GL_LINE);
            set_payload(m_Context, capabilities::gl_depth_test{.poly_offset{.factor{}, .units{-1.0}}}, capabilities::gl_polygon_offset_line{});
            m_ShaderProgram2DMonochrome.use();
            m_ShaderProgram2DMonochrome.set_uniform("colour", std::array{1.0f, 0.0f, 0.0f, 1.0f});
            m_Septagon.draw(std::array{texture_unit{2}, texture_unit{3}});
            gl_function{&GladGLContext::PolygonMode}(m_Context, GL_FRONT_AND_BACK, GL_FILL);
        }

        {
            gl_function{&GladGLContext::PolygonMode}(m_Context, GL_FRONT_AND_BACK, GL_POINT);
            gl_function{&GladGLContext::PointSize}(m_Context, 10);
            set_payload(m_Context, capabilities::gl_depth_test{.poly_offset{.factor{}, .units{-2.0}}}, capabilities::gl_polygon_offset_point{});
            m_ShaderProgram2DMonochrome.use();
            m_ShaderProgram2DMonochrome.set_uniform("colour", std::array{0.0f, 0.0f, 1.0f, 1.0f});
            m_Septagon.draw(std::array{texture_unit{2}, texture_unit{3}});
            gl_function{&GladGLContext::PolygonMode}(m_Context, GL_FRONT_AND_BACK, GL_FILL);
        }

        {
            set_payload(m_Context, capabilities::gl_depth_test{});
            m_ShaderProgram3DTextured.use();
            m_UpperHearts.draw(texture_unit{7});
        }

        {
            set_payload(
                m_Context,
                capabilities::gl_stencil_test{
                    .front{
                        .func{.comparison{comparison_mode::greater}, .reference_value{1}, .mask{255}},
                        .op{.on_failure{stencil_failure_mode::keep}, .on_pass_with_depth_failure{stencil_failure_mode::keep}, .on_pass_without_depth_failure{stencil_failure_mode::replace} },
                        .write_mask{255}
                    }
                }
            );
            m_DiscShaderProgram2D.use();
            m_Cutout.draw();
        }

        {
            set_payload(
                m_Context,
                capabilities::gl_stencil_test{
                    .front{
                        .func{.comparison{comparison_mode::greater}, .reference_value{1}, .mask{255}},
                        .op{.on_failure{stencil_failure_mode::keep}, .on_pass_with_depth_failure{stencil_failure_mode::keep}, .on_pass_without_depth_failure{stencil_failure_mode::keep} },
                        .write_mask{255}
                    }
                }
            );
            m_ShaderProgram2DTextured.use();
            m_LowerHearts.draw(texture_unit{8});
        }

        {
            set_payload(
                m_Context,
                capabilities::gl_stencil_test{
                    .front{
                        .func{.comparison{comparison_mode::equal}, .reference_value{1}, .mask{255}},
                        .op{.on_failure{stencil_failure_mode::keep}, .on_pass_with_depth_failure{stencil_failure_mode::keep}, .on_pass_without_depth_failure{stencil_failure_mode::keep} },
                        .write_mask{255}
                    }
                }
            );
            m_ShaderProgram2DTextured.use();
            m_Hexagon.draw(texture_unit{8});
        }

        {
            set_payload(
                m_Context,
                capabilities::gl_multi_sample{},
                capabilities::gl_sample_coverage{.coverage_val{0.75}, .invert{invert_sample_mask::no}},
                capabilities::gl_sample_alpha_to_coverage{}
            );
            m_ShaderProgram3DDoubleMonochrome.use();
            m_PartiallyTransparentQuadUpper.draw();
        }

        {
            set_payload(
                m_Context,
                capabilities::gl_blend{
                    .rgb{
                        .modes{.source{blend_mode::src_alpha}, .destination{blend_mode::one_minus_src_alpha}},
                        .algebraic_op{blend_eqn_mode::add}
                    },
                    .alpha{
                        .modes{.source{blend_mode::src_alpha}, .destination{blend_mode::one_minus_src_alpha}},
                        .algebraic_op{blend_eqn_mode::add}
                    },
                    .colour{}
                }
            );
            m_ShaderProgram3DDoubleMonochrome.use();
            m_PartiallyTransparentQuadLower.draw();
        }
    }
}