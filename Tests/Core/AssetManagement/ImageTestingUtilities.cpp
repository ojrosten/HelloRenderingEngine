////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageTestingUtilities.hpp"

#include <ranges>

namespace avocet::testing {

    template<class Fn>
        requires std::is_invocable_r_v<typename image_data::value_type, Fn , std::size_t, std::size_t>
    [[nodiscard]]
    image_data make_image(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, Fn fn) {
        const auto bytesPerChannel{sizeof(image_data::value_type)};

        const auto paddedRowSize{padded_row_size(w, channels, bytesPerChannel, rowAlignment)};
        const auto nominalRowSize{padded_row_size(w, channels, bytesPerChannel, alignment{1})};

        return {
            .data{
                  std::views::iota(0u, paddedRowSize * h)
                | std::views::transform(
                    [=](auto i) -> unsigned char {
                        if(const bool paddingBtye{i % paddedRowSize >= nominalRowSize}; paddingBtye)
                            return 0;

                        const auto row{i / paddedRowSize};
                        const auto channelIndex{(i - row * paddedRowSize) % channels.raw_value()};
                        return fn(row, channelIndex);
                    }
                  )
                | std::ranges::to<std::vector>()
            },
            .width{w},
            .height{h},
            .num_channels{channels}
        };

    }

    [[nodiscard]]
    image_data make_red(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, monochrome_intensity intensity) {
        auto fn{
            [intensity](std::size_t, std::size_t channelIndex){
                if(channelIndex == 3)
                     return intensity.alpha;

                return static_cast<unsigned char>(channelIndex ? 0 : intensity.red);
            }
        };

        return make_image(w, h, channels, rowAlignment, fn);
    }

    [[nodiscard]]
    image_data make_rgb_striped(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment) {
        return make_image(w, h, channels, rowAlignment, [channels](std::size_t row, std::size_t channelIndex) { return static_cast<unsigned char>((row % channels.raw_value()) == channelIndex ? 255 : 0); });
    }
}
