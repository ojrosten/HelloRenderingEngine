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

        void do_configure(const decorated_context& ctx, const framebuffer_texture_2d_configurator& config) {
            using value_type = texture_2d_configurator::value_type;

            gl_function{&GladGLContext::TexImage2D}(
                ctx,
                GL_TEXTURE_2D,
                0,
                to_gl_int(to_internal_format(config.format, config.decoding)),
                to_gl_sizei(config.dimensions.width),
                to_gl_sizei(config.dimensions.height),
                0,
                to_gl_enum(config.format),
                to_gl_enum(to_gl_type_specifier_v<value_type>),
                nullptr
            );

            gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            gl_function{&GladGLContext::TexParameteri}(ctx, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }

    [[nodiscard]]
    GLint extract_texture_2d_param(const decorated_context& ctx, GLenum paramName) {
        GLint param{};
        gl_function{&GladGLContext::GetTexLevelParameteriv}(ctx, GL_TEXTURE_2D, 0, paramName, &param);
        return param;
    }

    void texture_2d_lifecycle_events::configure(contextual_resource_view h, const configurator& config) {
        add_label(identifier, h, config.label);
        load_to_gpu(h.context(), config);
        if(config.parameter_setter)
            config.parameter_setter();
    }

    void framebuffer_texture_2d_lifecycle_events::configure(contextual_resource_view h, const configurator& config) {
        add_label(identifier, h, config.label);
        do_configure(h.context(), config);
    }

}