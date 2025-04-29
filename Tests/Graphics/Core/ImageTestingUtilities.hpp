////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/Graphics/Core/Image.hpp"

#include <ranges>

namespace avocet::testing {
    struct image_data {
        std::size_t width{}, height{}, num_channels{};
        std::vector<unsigned char> data;
    };

    inline image_data make_red(std::size_t width, std::size_t height, std::size_t channels) {
        image_data image{width, height, channels};
        image.data.resize(width * height * channels);
        for(auto i : std::views::iota(0u, image.data.size())) {
            if((i % 3) == 0) image.data[i] = 255;
        }

        return image;
    }
}

namespace sequoia::testing
{
    template<> struct value_tester<avocet::image>
    {
        using type = avocet::image;
        using image_data = avocet::testing::image_data;

        template<std::ranges::range R>
        static auto as_unsigned(R&& r) {
            return std::views::transform(r, [](auto c) -> unsigned int { return c; }) | std::ranges::to<std::vector>();
        }

        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const type& actual, const type& prediction)
        {
            check(equality, "Width",    logger, actual.width(),        prediction.width());
            check(equality, "Height",   logger, actual.height(),       prediction.height());
            check(equality, "Channels", logger, actual.num_channels(), prediction.num_channels());
            check(equality, "Data",     logger, as_unsigned(actual.span()), as_unsigned(prediction.span()));
        }

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const image_data& prediction)
        {
            check(equality, "Width",    logger, actual.width(),             prediction.width);
            check(equality, "Height",   logger, actual.height(),            prediction.height);
            check(equality, "Channels", logger, actual.num_channels(),      prediction.num_channels);
            check(equivalence, "Data",  logger, as_unsigned(actual.span()), as_unsigned(prediction.data));
        }
    };
}
