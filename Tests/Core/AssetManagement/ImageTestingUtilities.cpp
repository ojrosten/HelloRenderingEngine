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
    [[nodiscard]]
    image_data make_red(std::size_t w, std::size_t h, image_channels channels, std::size_t intensity, alignment rowAlignment) {
        const auto paddedRowSize{padded_row_size(w, channels, rowAlignment)};
        return {
            .data{
                  std::views::iota(0u, paddedRowSize * h)
                | std::views::transform(
                    [=](auto i) -> unsigned char {
                        const auto row{i / (w * channels.raw_value())};
                        const auto decrementedChannel{(i - paddedRowSize * row) % channels.raw_value()};
                        return static_cast<unsigned char>(decrementedChannel ? 0 : intensity);
                    }
                  )
                | std::ranges::to<std::vector>()
            },
            .width{w},
            .height{h},
            .num_channels{channels},
            .row_alignment{rowAlignment}
        };
    }

    [[nodiscard]]
    image_data make_rgb_striped(std::size_t w, std::size_t h, image_channels channels, unsigned char alpha, alignment rowAlignment) {
        const auto paddedRowSize{padded_row_size(w, channels, rowAlignment)};
        return {
            .data{
                  std::views::iota(0u, paddedRowSize * h)
                | std::views::transform(
                    [=](auto i) -> unsigned char {
                        const auto row{i / (w * channels.raw_value())};
                        const auto decrementedChannel{(i - paddedRowSize * row) % channels.raw_value()};
                        if(decrementedChannel == 3)
                            return alpha;

                        return static_cast<unsigned char>((row == decrementedChannel) ? 255 : 0);
                    }
                  )
                | std::ranges::to<std::vector>()
            },
            .width{w},
            .height{h},
            .num_channels{channels},
            .row_alignment{rowAlignment}
        };
    };
}
