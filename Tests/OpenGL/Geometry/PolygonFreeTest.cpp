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
    }
}
