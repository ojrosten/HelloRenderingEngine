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
    namespace {
        template<class Fn>
            requires std::is_invocable_r_v<typename image_data::value_type, Fn, std::size_t, std::size_t>
        [[nodiscard]]
        image_data make_image(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, Fn fn) {
            constexpr auto bytesPerChannel{sizeof(image_data::value_type)};

            const std::size_t paddedRowSize {padded_row_size(w, channels, bytesPerChannel, rowAlignment)},
                              nominalRowSize{padded_row_size(w, channels, bytesPerChannel, alignment{1})};
            return {
                .data{
                      std::views::iota(0u, safe_image_size(paddedRowSize, h))
                    | std::views::transform(
                        [=](auto i) -> unsigned char {
                            if(const bool paddingByte{i % paddedRowSize >= nominalRowSize}; paddingByte)
                                return 0;

                            const auto row{i / paddedRowSize};
                            const auto channelIndex{(i - paddedRowSize * row) % channels.raw_value()};
                            return fn(row, channelIndex);
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
    }

    [[nodiscard]]
    image_data make_red(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, monochrome_intensity intensity) {
        auto fn{
            [channels, intensity](std::size_t, std::size_t channelIndex){
                if((channelIndex == 3) || ((channels == colour_channels{2}) && (channelIndex == 1)))
                     return intensity.alpha;

                return static_cast<unsigned char>(channelIndex ? 0 : intensity.red);
            }
        };

        return make_image(w, h, channels, rowAlignment, fn);
    }

    [[nodiscard]]
    image_data make_rgb_striped(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment) {
        auto fn{
            [channels](std::size_t row, std::size_t channelIndex) {
                return static_cast<unsigned char>((row % channels.raw_value()) == channelIndex ? 255 : 0);
            }
        };

        return make_image(w, h, channels, rowAlignment, fn);
    }
}
