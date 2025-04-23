////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/OpenGL/Resources.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace fs = std::filesystem;

namespace avocet::opengl {
    namespace {
        struct image_format
        {
            GLint internal_format{};
            GLenum format{};
        };

        struct image
        {
            unsigned char& data;
            int width{}, height{}, num_channels{};
            image_format format{};
        };

        class [[nodiscard]] image_loader{
            image m_Image;

            [[nodiscard]]
            static image make(const fs::path& texture, vertical_flip flip) {
                stbi_set_flip_vertically_on_load(static_cast<bool>(flip));
                int width{}, height{}, numChannels{};

                if(auto pData{stbi_load(texture.generic_string().c_str(), &width, &height, &numChannels, 0)}; pData != nullptr)
                    return {.data{*pData}, .width{width}, .height{height}, .num_channels{numChannels}};

                throw std::runtime_error{std::format("Failed to load image {}", texture.generic_string())};
            }
        public:
            image_loader(const fs::path& texture, vertical_flip flip)
                : m_Image{make(texture, flip)}
            {
            }

            [[nodiscard]]
            const image& get_image() const noexcept { return m_Image; }

            ~image_loader() { stbi_image_free(&m_Image.data); }
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
            case 3:
                return {.internal_format{isLinear ? GL_RGB  : GL_SRGB},
                        .format{GL_RGB}};
            case 4:
                return {.internal_format{isLinear ? GL_RGBA : GL_SRGB_ALPHA},
                        .format{GL_RGBA}};
            }

            throw std::runtime_error{std::format("stbi_load: number of channels = {} not currently supported", numChannels)};
        }


        void load(const image_configuration& config) {
            image_loader loader{config.file, config.flip};
            const auto& im{loader.get_image()};
            const auto format{to_format(config.colour_space, im.num_channels)};

            gl_function{glTexImage2D}(GL_TEXTURE_2D, 0, format.internal_format, im.width, im.height, 0, format.format, GL_UNSIGNED_BYTE, &im.data);
        }
    }

    void texture_lifecycle_events::configure(const resource_handle& h, const configurator& config) {
        add_label(identifier, h, config.label);
        load(config.image_config);
    }
}