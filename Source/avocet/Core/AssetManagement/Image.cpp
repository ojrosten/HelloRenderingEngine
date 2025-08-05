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

    namespace {
        constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};

        constexpr bool multiplication_overflows(std::size_t x, std::size_t y) noexcept {
            return y ? maxVal / y < x : false;
        }
    }

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
        if(!bytesPerChannel)
            throw std::runtime_error{"padded_row_size: bytes per channel must be > 0"};

        if(multiplication_overflows(bytesPerChannel, channels.raw_value()))
            throw std::runtime_error{std::format("padded_row_size: channels ({}) * bytes per channel ({}) exceeds max allowed value, {}", channels, bytesPerChannel, maxVal)};

        const auto bytesPerTexel{channels.raw_value() * bytesPerChannel};
        if(multiplication_overflows(bytesPerTexel, width))
            throw std::runtime_error{std::format("padded_row_size: width ({}) * bytes per texel ({}) exceeds max allowed value, {}", width, bytesPerTexel, maxVal)};

        const auto nominalWidth{width * bytesPerTexel};
        const auto unpaddedBytes{nominalWidth % rowAlignment.raw_value()};
        if(!unpaddedBytes)
            return nominalWidth;

        if(maxVal - rowAlignment.raw_value() < nominalWidth - unpaddedBytes)
            throw std::runtime_error{std::format("padded_row_size: nominal row size ({}) aligned to ({}) bytes will be padded to exceed max allowed value, {}", nominalWidth, rowAlignment, maxVal)};

        return nominalWidth - unpaddedBytes + rowAlignment.raw_value();
    }

    [[nodiscard]]
    std::size_t safe_image_size(std::size_t width, std::size_t height) {
        if(multiplication_overflows(width, height))
            throw std::runtime_error{std::format("safe_image_size: width ({}) * height ({}) exceeds max allowed value, {}", width, height, maxVal)};

        return width * height;
    }
}
