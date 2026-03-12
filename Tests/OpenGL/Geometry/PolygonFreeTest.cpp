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

        template<gl_floating_point T>
        [[nodiscard]]
        std::string to_precision_name() {
            return std::same_as<T, GLfloat> ? "SinglePrecision" : "DoublePrecision";
        }

        [[nodiscard]]
        std::string to_dimensionality_name(dimensionality d) {
            return std::format("{}D", d);
        }

        [[nodiscard]]
        std::string to_uniform_name(std::size_t i) {
            return std::format("image{}", i);
        }

        template<gl_floating_point T>
        [[nodiscard]]
        std::string describe_poly(std::size_t numVerts, dimensionality dim, std::size_t numTextures) {
            return std::format("{}-gon embedded in D = {} with {} textures ({})", numVerts, dim, numTextures, to_precision_name<T>());
        }

        [[nodiscard]]
        texture_2d_configurator make_texture2d_configurator(const decorated_context& ctx, image_view picture) {
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

        template<class T, class U>
        struct replace {
            using type = U;
        };

        template<class T, class U>
        using replace_t = replace<T, U>::type;

        template<gl_floating_point T, std::size_t NumVertices, dimensionality Dim, class... Images>
            requires (std::convertible_to<Images, image_view> && ...)
        [[nodiscard]]
        polygon<T, NumVertices, Dim, texture_coordinates<replace_t<Images, T>>...> make_poly(const decorated_context& ctx, const Images&... images) {
            return {
                ctx,
                [](auto verts) {
                    for(auto& vert : verts) {
                        auto& coords{sequoia::get<0>(vert)};
                        coords = {away_from_zero(coords[0]), away_from_zero(coords[1])};
                    }

                    return verts;
                },
                std::array<texture_2d_configurator, sizeof...(Images)>{make_texture2d_configurator(ctx, images)...},
                make_label(describe_poly<T>(NumVertices, Dim, sizeof...(Images)))
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
        auto w{create_window({.extent{.width{1}, .height{1}}, .hiding{window_hiding_mode::on}})};

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

        test_polys<GLfloat,  3, dimensionality{2}, fbExtent>(fbo);
        test_polys<GLfloat,  3, dimensionality{3}, fbExtent>(fbo);
        test_polys<GLdouble, 3, dimensionality{2}, fbExtent>(fbo);
        test_polys<GLdouble, 3, dimensionality{3}, fbExtent>(fbo);

        test_polys<GLfloat,  4, dimensionality{2}, fbExtent>(fbo);
        test_polys<GLfloat,  4, dimensionality{3}, fbExtent>(fbo);
        test_polys<GLdouble, 4, dimensionality{2}, fbExtent>(fbo);
        test_polys<GLdouble, 4, dimensionality{3}, fbExtent>(fbo);
    }

    template<
        opengl::gl_floating_point T,
        std::size_t NumVerts,
        dimensionality Dim,
        discrete_extent Extent
    >
    void polygon_free_test::test_polys(const opengl::framebuffer_object& fbo) {
        test_poly(
            fbo,
            poly_data<T, NumVerts, Dim, 0, Extent> {
                .vertex_shader{"Identity.vs"},
                .frag_shader{"Monochrome.fs"},
                .images{},
                .bottom_prediction{128, 128}
            }
        );

        test_poly(
            fbo,
            poly_data<T, NumVerts, Dim, 1, Extent> {
                .vertex_shader{"IdentityTextured.vs"},
                .frag_shader{"Textured.fs"},
                .images{
                    {
                        128, 128,
                        128, 128,
                    }
                },
                .bottom_prediction{128, 128}
            }
        );

        test_poly(
            fbo,
            poly_data<T, NumVerts, Dim, 2, Extent> {
                .vertex_shader{"IdentityTwiceTextured.vs"},
                .frag_shader{"MixedTextures.fs"},
                .images{{
                    {
                        128, 0,
                        128, 0,
                    },
                    {
                         0,  128,
                         0,  128,
                    }
                }},
                .bottom_prediction{64, 64}
            }
        );
    }

    template<
        opengl::gl_floating_point T,
        std::size_t NumVerts,
        dimensionality Dim,
        discrete_extent Extent,
        std::size_t... Is
    >
    void polygon_free_test::test_poly(const opengl::framebuffer_object& fbo, const poly_data<T, NumVerts, Dim, sizeof...(Is), Extent>& polyData, std::index_sequence<Is...>) {
        const auto& ctx{fbo.context()};

        gl_function{&GladGLContext::ClearColor}(ctx, 0.0, 0.0, 0.0, 0.0);
        gl_function{&GladGLContext::Clear}(ctx, GL_COLOR_BUFFER_BIT);

        shader_program prog{
            ctx,
            get_vertex_shader_dir() / to_precision_name<T>() / to_dimensionality_name(Dim) / polyData.vertex_shader,
            get_frag_shader_dir()   / polyData.frag_shader
        };
        (prog.set_uniform(to_uniform_name(Is), to_gl_int(Is)), ...);

        auto poly{
            make_poly<T, NumVerts, Dim>(
                ctx,
                unique_image{polyData.images[Is], Extent.width, Extent.height, colour_channels{1}, alignment{1}}...
            )
        };

        gl_function{&GladGLContext::Viewport}(ctx, 0, 0, Extent.width, Extent.height);
        prog.use();

        constexpr auto numTextures{sizeof...(Is)};
        using array_t = std::array<texture_unit, numTextures>;
        poly.draw(array_t{texture_unit{Is}...});

        check(
            equivalence,
            describe_poly<T>(NumVerts, Dim, numTextures),
            fbo.extract_data(texture_format::red, alignment{1}),
            unique_image{
                build_prediction<NumVerts, Extent>(polyData.bottom_prediction),
                Extent.width,
                Extent.height,
                colour_channels{1},
                alignment{1}
            }
        );
    }

}
