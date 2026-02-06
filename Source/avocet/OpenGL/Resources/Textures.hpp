////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/AssetManagement/Image.hpp"
#include "avocet/Core/Geometry/Extent.hpp"

#include "avocet/OpenGL/ResourceInfrastructure/GenericResource.hpp"
#include "avocet/OpenGL/ResourceInfrastructure/Labels.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include <array>
#include <filesystem>
#include <functional>
#include <ranges>
#include <span>
#include <vector>

namespace avocet::opengl {
    enum class sampling_decoding : bool { none, srgb };

    enum class texture_internal_format : GLint {
        red   = GL_RED,
        rg    = GL_RG,
        rgb   = GL_RGB,
        srgb  = GL_SRGB,
        rgba  = GL_RGBA,
        srgba = GL_SRGB_ALPHA
    };

    enum class texture_format : GLenum {
        red  = GL_RED,
        rg   = GL_RG,
        rgb  = GL_RGB,
        rgba = GL_RGBA
    };

    [[nodiscard]]
    constexpr texture_internal_format to_internal_format(texture_format format, sampling_decoding colourSpace) {
        const bool noDecoding{colourSpace == sampling_decoding::none};

        switch(format) {
        case texture_format::red: return              texture_internal_format::red;
        case texture_format::rg: return              texture_internal_format::rg;
        case texture_format::rgb: return noDecoding ? texture_internal_format::rgb : texture_internal_format::srgb;
        case texture_format::rgba: return noDecoding ? texture_internal_format::rgba : texture_internal_format::srgba;
        }

        throw std::runtime_error{std::format("to_internal_format: unrecognized value of texture_format, {}", to_gl_enum(format))};
    }

    [[nodiscard]]
    constexpr texture_format to_texture_format(colour_channels numChannels) {
        switch(numChannels.raw_value()) {
            using enum texture_format;
        case 1: return red;
        case 2: return rg;
        case 3: return rgb;
        case 4: return rgba;
        }

        throw std::runtime_error{std::format("to_texture_format: {} colour channels requested but it must be in the range [1,4]", numChannels)};
    }

    struct texture_configurator_common {
        using value_type = GLubyte;

        sampling_decoding     decoding;
        std::function<void()> parameter_setter;
        optional_label        label{};
    };

    struct texture_2d_configurator {
        using value_type = texture_configurator_common::value_type;

        texture_configurator_common common_config;
        image_view                  data_view;
    };

    struct framebuffer_texture_2d_configurator {
        using value_type = texture_configurator_common::value_type;

        texture_configurator_common common_config;
        discrete_extent             extent;
        texture_format              format;
    };

    template<class T>
    inline constexpr bool defines_texture_configuration_v{
           has_configurator_type_v<T>
        && requires (contextual_resource_view crv, const T::configurator& config) {
               T::preliminary_configuration(crv, config);
           }
    };

    struct common_texture_2d_lifecycle_events {
        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(const decorated_context& ctx, raw_indices<N>& indices) { gl_function{&GladGLContext::GenTextures}(ctx, N, indices.data()); }

        template<std::size_t N>
        static void destroy(const decorated_context& ctx, const raw_indices<N>& indices) { gl_function{&GladGLContext::DeleteTextures}(ctx, N, indices.data()); }

        static void bind(contextual_resource_view crv) { gl_function{&GladGLContext::BindTexture}(crv.context(), GL_TEXTURE_2D, get_index(crv)); }

        template<class Self>
            requires defines_texture_configuration_v<Self>
        void configure(this const Self&, contextual_resource_view crv, const Self::configurator& config) {
            add_label(identifier, crv, config.common_config.label);
            Self::preliminary_configuration(crv, config);

            const auto commonConfig{Self::to_common_configurator(config)};

            using value_type = Self::configurator::value_type;
            gl_function{&GladGLContext::TexImage2D}(
                crv.context(),
                GL_TEXTURE_2D,
                0,
                to_gl_int(to_internal_format(commonConfig.format, config.common_config.decoding)),
                to_gl_sizei(commonConfig.extent.width),
                to_gl_sizei(commonConfig.extent.height),
                0,
                to_gl_enum(commonConfig.format),
                to_gl_enum(to_gl_type_specifier_v<value_type>),
                commonConfig.data.data()
             );

            if(config.common_config.parameter_setter)
                config.common_config.parameter_setter();
        }
    };

    template<gl_arithmetic T>
    struct common_texture_2d_configurator {
        texture_format     format;
        discrete_extent    extent;
        std::span<const T> data;
    };

    struct texture_2d_lifecycle_events : common_texture_2d_lifecycle_events {
        using configurator = texture_2d_configurator;

        [[nodiscard]]
        static common_texture_2d_configurator<configurator::value_type> to_common_configurator(const configurator& config) {
            return {
                .format{to_texture_format(config.data_view.num_channels())},
                .extent{
                    .width {static_cast<std::uint32_t>(config.data_view.width())},
                    .height{static_cast<std::uint32_t>(config.data_view.height())}
                },
                .data{config.data_view.span()}
            };
        }

        static void preliminary_configuration(contextual_resource_view crv, const configurator& config);

        //void configure(contextual_resource_view crv, const configurator& config);
    };

    struct framebuffer_texture_2d_lifecycle_events : common_texture_2d_lifecycle_events {
        using configurator = framebuffer_texture_2d_configurator;

        [[nodiscard]]
        static common_texture_2d_configurator<configurator::value_type> to_common_configurator(const configurator& config) {
            return {
                .format{config.format},
                .extent{config.extent},
                .data{}
            };
        }

        static void preliminary_configuration(contextual_resource_view, const configurator&) {}

        //void configure(contextual_resource_view crv, const configurator& config);
    };

    struct texture_unit {
        std::size_t index{};

        [[nodiscard]]
        GLenum gl_texture_unit() const { return GL_TEXTURE0 + to_gl_int(index); }

        [[nodiscard]]
        friend constexpr auto operator<=>(const texture_unit&, const texture_unit&) noexcept = default;
    };

    [[nodiscard]]
    GLint extract_texture_2d_param(const decorated_context& ctx, GLenum paramName);

    [[nodiscard]]
    constexpr colour_channels to_num_channels(texture_format format) {
        switch(format) {
            using enum texture_format;
        case red:  return colour_channels{1};
        case rg:   return colour_channels{2};
        case rgb:  return colour_channels{3};
        case rgba: return colour_channels{4};
        }

        throw std::runtime_error{std::format("to_num_channels: unrecognized value of texture_format {}", to_gl_enum(format))};
    }

    [[nodiscard]]
    constexpr GLint to_ogl_alignment(alignment rowAlignment) {
        if(rowAlignment.raw_value() > 8)
            throw std::runtime_error{std::format("Row alignment of {} bytes requested, but OpenGL only supports 1, 2, 4 and 8 bytes", rowAlignment)};

        return to_gl_int(rowAlignment.raw_value());
    }

    template<class LifeEvents>
        requires has_resource_lifecycle_events_v<num_resources{1}, LifeEvents>
    class generic_texture_2d : public generic_resource<num_resources{1}, LifeEvents> {
    public:
        using base_type         = generic_resource<num_resources{1}, LifeEvents>;
        using configurator_type = base_type::configurator_type;
        using value_type        = configurator_type::value_type;

        generic_texture_2d(const activating_context& ctx, const configurator_type& textureConfig)
            : base_type{ctx, {textureConfig}}
        {}

        [[nodiscard]]
        unique_image extract_data(this const generic_texture_2d& self, texture_format format, alignment rowAlignment) {
            base_type::do_bind(self);

            const auto& ctx{self.context()};
            const auto width{static_cast<std::size_t>(extract_texture_2d_param(ctx, GL_TEXTURE_WIDTH))},
                      height{static_cast<std::size_t>(extract_texture_2d_param(ctx, GL_TEXTURE_HEIGHT))};

            const auto numChannels{to_num_channels(format)};
            const auto size{safe_image_size(padded_row_size(width, numChannels, sizeof(value_type), rowAlignment), height)};

            std::vector<value_type> texture(size);

            gl_function{&GladGLContext::PixelStorei}(ctx, GL_PACK_ALIGNMENT, to_ogl_alignment(rowAlignment));

            gl_function{&GladGLContext::GetTexImage}(
                ctx,
                GL_TEXTURE_2D,
                0,
                to_gl_enum(format),
                to_gl_enum(to_gl_type_specifier_v<value_type>),
                texture.data()
           );

            return {texture, width, height, numChannels, rowAlignment};
        }

        void bind(this const generic_texture_2d& self, texture_unit unit) {
            gl_function{&GladGLContext::ActiveTexture}(self.context(), unit.gl_texture_unit());
            base_type::do_bind(self);
        }
    private:
        friend class framebuffer_object;

        [[nodiscard]]
        contextual_resource_view contextual_handle() const noexcept {
            return base_type::contextual_handle(index<0>{});
        }
    };

    class texture_2d : public generic_texture_2d<texture_2d_lifecycle_events>{
    public:
        using generic_texture_2d<texture_2d_lifecycle_events>::generic_texture_2d;
    };

    class framebuffer_texture_2d : public generic_texture_2d<framebuffer_texture_2d_lifecycle_events> {
    public:
        using generic_texture_2d<framebuffer_texture_2d_lifecycle_events>::generic_texture_2d;
    };
}
