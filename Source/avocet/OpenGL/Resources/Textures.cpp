////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Resources/Textures.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        constexpr texture_internal_format to_internal_format(texture_format format, sampling_decoding colourSpace) {
            const bool noDecoding{colourSpace == sampling_decoding::none};

            switch(format) {
            case texture_format::red  : return              texture_internal_format::red;
            case texture_format::rg   : return              texture_internal_format::rg;
            case texture_format::rgb  : return noDecoding ? texture_internal_format::rgb  : texture_internal_format::srgb;
            case texture_format::rgba : return noDecoding ? texture_internal_format::rgba : texture_internal_format::srgba;
            }

            throw std::runtime_error{std::format("to_internal_format: unrecognized value of texture_format, {}", to_gl_enum(format))};
        }

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

        void load_to_gpu(const decorated_context& ctx, const texture_2d_configurator& config) {
            gl_function{&GladGLContext::PixelStorei}(ctx, GL_UNPACK_ALIGNMENT, to_ogl_alignment(config.data_view.row_alignment()));

            const auto format{to_texture_format(config.data_view.num_channels())};
            using value_type = texture_2d_configurator::value_type;

            gl_function{&GladGLContext::TexImage2D}(
                ctx,
                GL_TEXTURE_2D,
                0,
                to_gl_int(to_internal_format(format, config.decoding)),
                to_gl_sizei(config.data_view.width()),
                to_gl_sizei(config.data_view.height()),
                0,
                to_gl_enum(format),
                to_gl_enum(to_gl_type_specifier_v<value_type>),
                config.data_view.span().data()
            );
        }

        [[nodiscard]]
        GLint extract_texture_2d_param(const decorated_context& ctx, GLenum paramName) {
            GLint param{};
            gl_function{&GladGLContext::GetTexLevelParameteriv}(ctx, GL_TEXTURE_2D, 0, paramName, &param);
            return param;
        }
    }

    void texture_2d_lifecycle_events::configure(contextual_resource_view h, const configurator& config) {
        add_label(identifier, h, config.label);
        load_to_gpu(h.context(), config);
        if(config.parameter_setter)
            config.parameter_setter();
    }

    [[nodiscard]]
    unique_image texture_2d::do_extract_data(const texture_2d& tex2d, texture_format format, alignment rowAlignment) {
        texture_2d::do_bind(tex2d);

        const auto& ctx{tex2d.context()};
        using value_type = texture_2d::value_type;
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
}