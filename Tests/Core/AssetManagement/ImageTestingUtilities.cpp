////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "ImageTestingUtilities.hpp"

#include <ranges>

namespace avocet::testing {

    [[nodiscard]]
    image_data make_red(std::size_t width, std::size_t height, std::size_t channels) {
        image_data image{width, height, channels};
        image.data.resize(width * height * channels);
        for(auto i : std::views::iota(0u, image.data.size())) {
            if((i % 3) == 0) image.data[i] = 255;
        }

        return image;
    }

    [[nodiscard]]
    image_data make_striped(std::size_t w, std::size_t h, std::size_t channels) {
        return {
            .width{w},
            .height{h},
            .num_channels{channels},
            .data{
                  std::views::iota(0u, w * h * channels)
                | std::views::transform(
                    [=](auto i) -> unsigned char {
                        const auto row{i / (w * channels)};
                        const auto channel{i % channels};
                        return static_cast<unsigned char>((row == channel) ? 255 : 0);
                    }
                  )
                | std::ranges::to<std::vector>()
            }
        };
    };
}
