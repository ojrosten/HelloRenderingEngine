////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/Core/Images.hpp"
#include <format>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace avocet {
    namespace fs = std::filesystem;

    image_loader::image_loader(const fs::path& texture, vertically_flipped flip)
        : m_Image{make(texture, flip)}
    {}

    image_loader::~image_loader() { stbi_image_free(m_Image.data.data()); }

    [[nodiscard]]
    image image_loader::make(const fs::path& texture, vertically_flipped flip) {
        stbi_set_flip_vertically_on_load(static_cast<bool>(flip));
        int width{}, height{}, numChannels{};

        if(auto pData{stbi_load(texture.generic_string().c_str(), &width, &height, &numChannels, 0)}; pData != nullptr)
            return {.data{std::span{pData, width * height * numChannels * sizeof(unsigned char)}}, .width{width}, .height{height}, .num_channels{numChannels}};

        throw std::runtime_error{std::format("Failed to load image {}", texture.generic_string())};
    }
}
