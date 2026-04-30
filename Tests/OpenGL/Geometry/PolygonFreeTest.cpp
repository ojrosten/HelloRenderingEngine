////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "PolygonFreeTest.hpp"
#include "avocet/OpenGL/Geometry/Polygon.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

#include "Core/AssetManagement/ImageTestingUtilities.hpp"

#include "sequoia/FileSystem/FileSystem.hpp"

namespace avocet::testing
{
    using namespace opengl;
    namespace fs = std::filesystem;
 
    namespace{
        [[nodiscard]]
        std::string make_label(std::string_view name, std::source_location loc = std::source_location::current()) {
            return std::format("{} created at {} line {}", name, sequoia::back(fs::path{loc.file_name()}).string(), loc.line());

        }

        template<gl_floating_point T>
        [[nodiscard]]
        T away_from_zero(T t) { return t > 0 ? std::ceil(t) : std::floor(t); };

        template<gl_floating_point CoordsValueType, gl_floating_point... TextureCoordsValueTypes>
        [[nodiscard]]
        std::string to_precision_name() {
            if constexpr(std::same_as<CoordsValueType, GLfloat> && (std::same_as<TextureCoordsValueTypes, GLfloat> && ...))
                return "SinglePrecision";
            else if constexpr(std::same_as<CoordsValueType, GLdouble> && (std::same_as<TextureCoordsValueTypes, GLdouble> && ...))
                return "DoublePrecision";
            else if constexpr(std::same_as<CoordsValueType, GLdouble> && (std::same_as<TextureCoordsValueTypes, GLfloat> &&... ))
                return "MixedPrecision";
            else
                static_assert(false, "Test Materials must be prepared for any additional cases");
        }

        [[nodiscard]]
        std::string to_dimensionality_name(dimensionality d) {
            return std::format("{}D", d);
        }

        [[nodiscard]]
        std::string to_uniform_name(std::size_t i) {
            return std::format("image{}", i);
        }

        template<gl_floating_point CoordsValueType, gl_floating_point... TextureCoordsValueTypes>
        [[nodiscard]]
        std::string describe_poly(std::size_t numVerts, dimensionality dim) {
            return std::format("{}-gon embedded in D = {} with {} textures ({})", numVerts, dim, sizeof...(TextureCoordsValueTypes), to_precision_name<CoordsValueType, TextureCoordsValueTypes...>());
        }

        [[nodiscard]]
        texture_2d_configurator make_texture2d_configurator(const resourceful_context& ctx, image_view picture) {
            return {
                .common_config{
                    .decoding{sampling_decoding::none},
                    .parameter_setter{
                        [&ctx]() {
                            gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                            gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        }
                    },
                    .label{}
                 },
                .data_view{picture},
            };
        }


        template<gl_floating_point CoordsValueType, std::size_t NumVertices, dimensionality Dim, gl_floating_point... TextureCoordsValueTypes>
        [[nodiscard]]
        polygon<CoordsValueType, NumVertices, Dim, texture_coordinates<TextureCoordsValueTypes>...> make_poly(const resourceful_context& ctx, const std::array<unique_image, sizeof...(TextureCoordsValueTypes)>& images) {
            return {
                ctx,
                [](auto verts) {
                    for(auto& vert : verts) {
                        auto& coords{sequoia::get<0>(vert)};
                        coords = {away_from_zero(coords[0]), away_from_zero(coords[1])};
                    }

                    return verts;
                },
                to_array(images, [&ctx](image_view iv) { return make_texture2d_configurator(ctx, iv); }),
                make_label(describe_poly<CoordsValueType, TextureCoordsValueTypes...>(NumVertices, Dim))
            };
        }

        template<std::size_t NumVerts, discrete_extent Extent>
            requires (3 <= NumVerts) && (NumVerts <= 4)
        [[nodiscard]]
        constexpr std::array<unsigned char, Extent.width * Extent.height> build_prediction(const std::array<unsigned char, Extent.width>& bottomPrediction) {
            std::array<unsigned char, Extent.width * Extent.height> prediction{};
            std::ranges::copy(bottomPrediction, prediction.begin());
            if constexpr(NumVerts == 4) {
                std::ranges::copy(bottomPrediction, std::ranges::next(prediction.begin(), Extent.width));
            }

            return prediction;
        }
    }

    [[nodiscard]]
    std::filesystem::path polygon_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void polygon_free_test::run_tests()
    {
        using namespace curlew;

        const std::vector<message_id> acceptableWarnings{
            [setup{test_window_manager::find_rendering_setup()}]() -> std::vector<message_id> {
                if(curlew::is_nvidia(setup.renderer))
                    return {message_id{131218}};

                return {};
            }()
        };

        auto w{
            create_window(
                {
                    .extent{.width{1}, .height{1}},
                    .hiding{window_hiding_mode::on},
                    .debug_mode{debugging_mode::dynamic},
                    .prologue{},
                    .epilogue{standard_error_checker{num_messages{10}, default_debug_info_processor{acceptableWarnings}}},
                }
            )
        };

        constexpr discrete_extent fbExtent{.width{2}, .height{2}};
        framebuffer_object
            fbo{
                w.context(),
                fbo_configurator{.label{}},
                framebuffer_texture_2d_configurator{
                    .common_config{},
                    .format{texture_format::red},
                    .extent{fbExtent}
                }
        };

        test_polys<GLfloat , 3, dimensionality{2}, GLfloat , fbExtent>(fbo);
        test_polys<GLfloat , 3, dimensionality{3}, GLfloat , fbExtent>(fbo);
        test_polys<GLdouble, 3, dimensionality{2}, GLdouble, fbExtent>(fbo);
        test_polys<GLdouble, 3, dimensionality{3}, GLdouble, fbExtent>(fbo);

        test_polys<GLfloat , 4, dimensionality{2}, GLfloat , fbExtent>(fbo);
        test_polys<GLfloat , 4, dimensionality{3}, GLfloat , fbExtent>(fbo);
        test_polys<GLdouble, 4, dimensionality{2}, GLdouble, fbExtent>(fbo);
        test_polys<GLdouble, 4, dimensionality{3}, GLdouble, fbExtent>(fbo);

        test_polys<GLdouble, 3, dimensionality{2}, GLfloat , fbExtent>(fbo);
        test_polys<GLdouble, 3, dimensionality{3}, GLfloat , fbExtent>(fbo);
        test_polys<GLdouble, 4, dimensionality{2}, GLfloat , fbExtent>(fbo);
        test_polys<GLdouble, 4, dimensionality{3}, GLfloat , fbExtent>(fbo);
    }

    template<
        gl_floating_point CoordsValueType,
        std::size_t NumVerts,
        dimensionality Dim,
        gl_floating_point TextureCoordsValueType,
        discrete_extent Extent
    >
    void polygon_free_test::test_polys(const opengl::framebuffer_object& fbo) {
        test_poly(
            fbo,
            poly_data<CoordsValueType, NumVerts, Dim, Extent> {
                .vertex_shader{"Identity.vs"},
                .frag_shader{"Monochrome.fs"},
                .images{},
                .bottom_prediction{128, 128}
            }
        );

        test_poly(
            fbo,
            poly_data<CoordsValueType, NumVerts, Dim, Extent, TextureCoordsValueType> {
                .vertex_shader{"IdentityTextured.vs"},
                .frag_shader{"Textured.fs"},
                .images{
                    {
                        128, 64,
                        128, 64,
                    }
                },
                .bottom_prediction{128, 64}
            }
        );

        test_poly(
            fbo,
            poly_data<CoordsValueType, NumVerts, Dim, Extent, TextureCoordsValueType, TextureCoordsValueType> {
                .vertex_shader{"IdentityTwiceTextured.vs"},
                .frag_shader{"MixedTextures.fs"},
                .images{{
                    {
                        128, 0,
                        128, 0,
                    },
                    {
                         0,  64,
                         0,  64,
                    }
                }},
                .bottom_prediction{64, 32}
            }
        );
    }

    template<
        gl_floating_point CoordsValueType,
        std::size_t NumVerts,
        dimensionality Dim,
        discrete_extent Extent,
        gl_floating_point... TextureCoordsValueTypes,
        std::size_t... Is
    >
    void polygon_free_test::test_poly(const opengl::framebuffer_object& fbo, const poly_data<CoordsValueType, NumVerts, Dim, Extent, TextureCoordsValueTypes...>& polyData, std::index_sequence<Is...>) {
        const auto& ctx{fbo.context()};

        gl_function{&GladGLContext::ClearColor}(ctx, 0.0, 0.0, 0.0, 0.0);
        gl_function{&GladGLContext::Clear}(ctx, GL_COLOR_BUFFER_BIT);

        shader_program prog{
            ctx,
            get_vertex_shader_dir() / to_precision_name<CoordsValueType, TextureCoordsValueTypes...>() / to_dimensionality_name(Dim) / polyData.vertex_shader,
            get_frag_shader_dir()   / polyData.frag_shader
        };
        (prog.set_uniform(to_uniform_name(Is), to_gl_int(Is)), ...);

        constexpr auto numTextures{sizeof...(TextureCoordsValueTypes)};

        auto poly{
            make_poly<CoordsValueType, NumVerts, Dim, TextureCoordsValueTypes...>(
                ctx,
                std::array<unique_image, numTextures>{unique_image{polyData.images[Is], Extent, colour_channels{1}, alignment{1}}...}
            )
        };

        gl_function{&GladGLContext::Viewport}(ctx, 0, 0, Extent.width, Extent.height);
        prog.use();

        using array_t = std::array<texture_unit, numTextures>;
        poly.draw(array_t{texture_unit{Is}...});

        check(
            equivalence,
            describe_poly<CoordsValueType, TextureCoordsValueTypes...>(NumVerts, Dim),
            fbo.extract_data(texture_format::red, alignment{1}),
            unique_image{
                build_prediction<NumVerts, Extent>(polyData.bottom_prediction),
                Extent,
                colour_channels{1},
                alignment{1}
            }
        );
    }

}
