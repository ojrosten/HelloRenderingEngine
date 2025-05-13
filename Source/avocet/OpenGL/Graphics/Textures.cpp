////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Graphics/Textures.hpp"

namespace avocet::opengl {
    namespace {
        struct image_format
        {
            texture_internal_format internal_format{};
            texture_format format{};
        };

        [[nodiscard]]
        image_format to_format(colour_space_flavour colourSpace, image_channels numChannels)
        {
            const bool isLinear{colourSpace == colour_space_flavour::linear};
            switch(numChannels.raw_value())
            {
            case 1:
                return {.internal_format{texture_internal_format::red},
                        .format{texture_format::red}};
            case 2:
                return {.internal_format{texture_internal_format::rg},
                        .format{texture_format::rg}};
            case 3:
                return {.internal_format{isLinear ? texture_internal_format::rgb : texture_internal_format::srgb},
                        .format{texture_format::rgb}};
            case 4:
                return {.internal_format{isLinear ? texture_internal_format::rgba : texture_internal_format::srgba},
                        .format{texture_format::rgba}};
            }

            throw std::runtime_error{std::format("{} channels requested, but it must be in the range [1,4]", numChannels)};
        }

        void load_to_gpu(const texture_2d_configuration& config) {
            const auto format{to_format(config.colour_space, config.data.num_channels())};

            glPixelStorei(GL_UNPACK_ALIGNMENT, static_cast<int>(config.data.row_alignment().raw_value()));

            gl_function{glTexImage2D}(
                GL_TEXTURE_2D,
                0,
                static_cast<GLint>(format.internal_format),
                static_cast<int>(config.data.width()),
                static_cast<int>(config.data.height()),
                0,
                to_gl_enum(format.format),
                to_gl_enum(to_gl_type_specifier_v<texture_2d_configuration::value_type>),
                config.data.span().data()
            );
            if(config.parameter_setter) config.parameter_setter();
        }
    }

    void texture_lifecycle_events::configure(const resource_handle& h, const configuration& config) {
        add_label(identifier, h, config.label);
        load_to_gpu(config);
    }

    [[nodiscard]]
    image extract_image(const texture_2d& tex2d, texture_format format) {
        texture_2d::base_type::do_bind(tex2d);
        const GLint width{texture_2d::extract_texture_2d_param(GL_TEXTURE_WIDTH)}, height{texture_2d::extract_texture_2d_param(GL_TEXTURE_HEIGHT)};
        const image_channels numChannels{to_num_channels(format)};

        using value_type = texture_2d::value_type;
        const auto size{padded_row_size(width, numChannels, tex2d.m_RowAlignment) * height};
        std::vector<value_type> texture(size);
        glPixelStorei(GL_PACK_ALIGNMENT, static_cast<int>(tex2d.m_RowAlignment.raw_value()));
        gl_function{glGetTexImage}(GL_TEXTURE_2D, 0, to_gl_enum(format), to_gl_enum(to_gl_type_specifier_v<value_type>), texture.data());
        return {texture, static_cast<std::size_t>(width), static_cast<std::size_t>(height), numChannels, tex2d.m_RowAlignment};
    }
}