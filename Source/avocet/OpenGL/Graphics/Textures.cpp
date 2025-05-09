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
        image_format to_format(colour_space_flavour colourSpace, std::size_t numChannels)
        {
            const bool isLinear{colourSpace == colour_space_flavour::linear};
            switch(numChannels)
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
    }

    void load_texture_2d_to_gpu(const texture_2d_configuration& config) {
        image im{config.image_config.file(), config.image_config.flipped()};
        const auto format{to_format(config.image_config.colour_space(), im.num_channels())};

        gl_function{glTexImage2D}(
            GL_TEXTURE_2D,
            0,
            static_cast<GLint>(format.internal_format),
            static_cast<int>(im.width()),
            static_cast<int>(im.height()),
            0,
            to_gl_enum(format.format),
            to_gl_enum(to_gl_type_specifier_v<texture_2d_configuration::value_type>),
            im.span().data()
        );
        if(config.parameter_setter) config.parameter_setter();
    }
}