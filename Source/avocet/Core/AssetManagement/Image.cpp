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

    [[nodiscard]]
    std::size_t padded_row_size(std::size_t width, colour_channels channels, alignment rowAlignment, std::size_t bytesPerChannel) {
        constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};

        const auto bytesPerTexel{channels.raw_value() * bytesPerChannel};
        if(bytesPerTexel && (maxVal / bytesPerTexel < width))
            throw std::runtime_error{std::format("padded_row_size: width ({}) * channels ({}) * bytes per channel ({}) exceed the maximum allowed value {}", width, channels.raw_value(), bytesPerChannel, maxVal)};

        const std::size_t nominalRowSize{width * bytesPerTexel};

        if(const std::size_t overhangingBytes{nominalRowSize % rowAlignment.raw_value()}; overhangingBytes) {
            if(nominalRowSize - overhangingBytes > maxVal - rowAlignment.raw_value())
                throw std::runtime_error{std::format("padded_row_size: nominal row size ({}) aligned to a ({}) byte boundary will exceed the maxmimu allowed value {}", nominalRowSize, rowAlignment.raw_value(), maxVal)};

            return nominalRowSize - overhangingBytes + rowAlignment.raw_value();
        }

        return nominalRowSize;
    }


    void unique_image::file_unloader::operator()(value_type* ptr) const {
        stbi_image_free(ptr);
    }

    void validate(std::size_t paddedRowSize, std::size_t height, std::size_t size) {
        if(paddedRowSize * height != size)
            throw std::runtime_error{std::format("unique_image size {} is not a multiple of the height {} and padded row size {}", size, paddedRowSize, height)};
    }

    [[nodiscard]]
    unique_image unique_image::make(const std::filesystem::path& texturePath, flip_vertically flip, const std::optional<colour_channels> requestedChannels) {
        if(!fs::exists(texturePath))
            throw std::runtime_error{std::format("unique_image: texture {} not found", texturePath.generic_string())};

        if(requestedChannels)
        {
            if((requestedChannels.value() < colour_channels{1}) || (requestedChannels.value() > colour_channels{4}))
                throw std::runtime_error{std::format("unique_image: invalid number of channels {} requested", requestedChannels.value())};
        }

        stbi_set_flip_vertically_on_load_thread(static_cast<bool>(flip));

        int width{}, height{}, channels{}, channelSelection{requestedChannels ? static_cast<int>(requestedChannels->raw_value()) : 0};
        auto pData{stbi_load(texturePath.generic_string().c_str(), &width, &height, &channels, channelSelection)};
        if(!pData)
            throw std::runtime_error{std::format("unique_image: texture {} did not load", texturePath.generic_string())};

        const auto actualChannels{static_cast<int>(channelSelection ? channelSelection : channels)};
        return {pData, width, height, actualChannels, alignment{1}};
    }
}
