////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/Textures.hpp"

namespace avocet::opengl {
    namespace {
        struct image_format
        {
            GLint internal_format{};
            GLenum format{};
        };

        [[nodiscard]]
        image_format to_format(colour_space_flavour colourSpace, int numChannels)
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

    void load_to_texture(const image_configuration& config, texture_flavour textureFlavour) {
        image_loader loader{config.file, config.flip};
        const auto& im{loader.get_image()};
        const auto format{to_format(config.colour_space, im.num_channels)};

        gl_function{glTexImage2D}(to_gl_enum(textureFlavour), 0, format.internal_format, im.width, im.height, 0, format.format, GL_UNSIGNED_BYTE, im.data.data());
        glGenerateMipmap(to_gl_enum(textureFlavour));
    }
}