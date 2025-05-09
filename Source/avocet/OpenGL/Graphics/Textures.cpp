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
            GLint internal_format{};
            GLenum format{};
        };

        [[nodiscard]]
        image_format to_format(colour_space_flavour colourSpace, std::size_t numChannels)
        {
            const bool isLinear{colourSpace == colour_space_flavour::linear};
            switch(numChannels)
            {
            case 1:
                return {.internal_format{GL_RED},
                        .format{GL_RED}};
            case 2:
                return {.internal_format{GL_RG},
                        .format{GL_RG}};
            case 3:
                return {.internal_format{isLinear ? GL_RGB  : GL_SRGB},
                        .format{GL_RGB}};
            case 4:
                return {.internal_format{isLinear ? GL_RGBA : GL_SRGB_ALPHA},
                        .format{GL_RGBA}};
            }

            throw std::runtime_error{std::format("{} channels requested, but it must be in the range [1,4]", numChannels)};
        }
    }

    void load_texture_2d_to_gpu(const texture_2d_configuration& config) {
        image im{config.image_config.file(), config.image_config.flipped()};
        const auto format{to_format(config.image_config.colour_space(), im.num_channels())};

        gl_function{glTexImage2D}(GL_TEXTURE_2D, 0, format.internal_format, static_cast<int>(im.width()), static_cast<int>(im.height()), 0, format.format, GL_UNSIGNED_BYTE, im.span().data());
        if(config.parameter_setter) config.parameter_setter();
    }
}