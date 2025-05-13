////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Core/AssetManagement/Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace avocet {
    namespace fs = std::filesystem;

    void image::file_unloader::operator()(value_type* ptr) const {
        stbi_image_free(ptr);
    }

    void validate(std::size_t paddedRowSize, std::size_t height, std::size_t size) {
        if(paddedRowSize * height != size)
            throw std::runtime_error{std::format("image size {} is not a multiple of the height {} and padded row size {}", size, paddedRowSize, height)};
    }

    [[nodiscard]]
    image image::make(const std::filesystem::path& texturePath, flip_vertically flip, const std::optional<image_channels> requestedChannels) {
        if(!fs::exists(texturePath))
            throw std::runtime_error{std::format("image: texture {} not found", texturePath.generic_string())};

        if(requestedChannels)
        {
            if((requestedChannels.value() < image_channels{1}) || (requestedChannels.value() > image_channels{4}))
                throw std::runtime_error{std::format("image: invalid number of channels {} requested", requestedChannels.value())};
        }

        const auto outputChannels{requestedChannels.value_or(image_channels{})};

        stbi_set_flip_vertically_on_load_thread(static_cast<bool>(flip));

        int width{}, height{}, channels{};
        auto pData{stbi_load(texturePath.generic_string().c_str(), &width, &height, &channels, static_cast<int>(outputChannels.raw_value()))};
        if(!pData)
            throw std::runtime_error{std::format("image: texture {} did not load", texturePath.generic_string())};

        return {pData, width, height, channels, alignment{1}};
    }
}
