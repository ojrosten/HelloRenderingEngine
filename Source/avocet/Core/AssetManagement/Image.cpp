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

    void unique_image::file_unloader::operator()(value_type* ptr) const {
        stbi_image_free(ptr);
    }

    [[nodiscard]]
    unique_image unique_image::make(const std::filesystem::path& texturePath, flip_vertically flip, std::optional<colour_channels> requestedChannels) {
        if(!fs::exists(texturePath))
            throw std::runtime_error{std::format("unique_image: texture {} not found", texturePath.generic_string())};

        if(requestedChannels)
        {
            if((requestedChannels.value() < colour_channels{1}) || (requestedChannels.value() > colour_channels{4}))
                throw std::runtime_error{std::format("unique_image: {} channels requested, but only 1--4 supported by stbi_load", requestedChannels.value().raw_value())};
        }

        stbi_set_flip_vertically_on_load_thread(static_cast<bool>(flip));

        int width{}, height{}, channels{}, channelsSelection{requestedChannels ? static_cast<int>(requestedChannels.value().raw_value()) : 0};
        auto pData{stbi_load(texturePath.generic_string().c_str(), &width, &height, &channels, channelsSelection)};
        if(!pData)
            throw std::runtime_error{std::format("unique_image: texture {} did not load", texturePath.generic_string())};

        const auto actualChannels{static_cast<int>(channelsSelection ? channelsSelection : channels)};
        return {pData, width, height, actualChannels, alignment{1}};
    }

    [[nodiscard]]
    std::size_t padded_row_size(std::size_t width, colour_channels channels, std::size_t bytesPerChannel, alignment rowAlignment) {
        constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};
        if(!bytesPerChannel)
            throw std::out_of_range{"padded_row_size requires a non-zero number of bytes per channel"};

        if(maxVal / bytesPerChannel < channels.raw_value())
            throw std::out_of_range{std::format("padded_row_size: bytesPerChannel ({}) * channels ({}) exceeed maximum allowed value {}", bytesPerChannel, channels.raw_value(), maxVal)};

        const auto bytesPerTexel{channels.raw_value() * bytesPerChannel};
        if(bytesPerTexel && (maxVal / bytesPerTexel < width))
            throw std::out_of_range{std::format("padded_row_size: bytesPerTexel ({}) * width ({}) exceeed maximum allowed value {}", bytesPerTexel, width, maxVal)};

        const auto nominalWidth{width * channels.raw_value() * bytesPerChannel};
        const auto unpaddedBytes{nominalWidth % rowAlignment.raw_value()};

        if(!unpaddedBytes) return nominalWidth;

        if(nominalWidth - unpaddedBytes > maxVal - rowAlignment.raw_value())
            throw std::out_of_range{std::format("padded_row_size: nominal width ({}) aligned to a ({}) byte boundary will be padded to exceeed the maximum allowed value {}", nominalWidth, rowAlignment.raw_value(), maxVal)};

        return nominalWidth - unpaddedBytes + rowAlignment.raw_value();
    }
}
