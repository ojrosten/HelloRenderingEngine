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
    image_data make_red(std::size_t w, std::size_t h, colour_channels channels) {
        return {
            .data{
                  std::views::iota(0u, w * h * channels.raw_value())
                | std::views::transform(
                    [=](auto i) -> unsigned char {
                        const auto channelIndex{i % channels.raw_value()};
                        return static_cast<unsigned char>(channelIndex ? 0 : 255);
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
    image_data make_rgb_striped(std::size_t w, std::size_t h, colour_channels channels) {
        return {
            .data{
                  std::views::iota(0u, w * h * channels.raw_value())
                | std::views::transform(
                    [=](auto i) -> unsigned char {
                        const auto row{i / (w * channels.raw_value())};
                        const auto channelIndex{i % channels.raw_value()};
                        return static_cast<unsigned char>((row % channels.raw_value()) == channelIndex ? 255 : 0);
                    }
                  )
                | std::ranges::to<std::vector>()
            },
            .width{w},
            .height{h},
            .num_channels{channels}
        };
    };
}
