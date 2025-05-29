////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "ColourChannelsTestingUtilities.hpp"

#include "sequoia/TestFramework/FreeCheckers.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

#include <ranges>

namespace avocet::testing {
    struct image_data {
        using value_type = unsigned char;

        std::vector<value_type> data;
        std::size_t width{}, height{}, num_channels{};
    };

    [[nodiscard]]
    image_data make_red(std::size_t width, std::size_t height, std::size_t channels);

    [[nodiscard]]
    image_data make_rgb_striped(std::size_t w, std::size_t h, std::size_t channels);
}

namespace sequoia::testing
{
    template<> struct value_tester<avocet::unique_image>
    {
        using image_data = avocet::testing::image_data;
        using value_type = image_data::value_type;

        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const avocet::unique_image& actual, const avocet::unique_image& prediction)
        {
            check(equality, "Width",    logger, actual.width(),        prediction.width());
            check(equality, "Height",   logger, actual.height(),       prediction.height());
            check(equality, "Channels", logger, actual.num_channels(), prediction.num_channels());
            check(equality, "Data",     logger, actual.span(),         prediction.span());
        }

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const avocet::unique_image& actual, const image_data& prediction)
        {
            check(equality,    "Width",    logger, actual.width(),        prediction.width);
            check(equality,    "Height",   logger, actual.height(),       prediction.height);
            check(equality,    "Channels", logger, actual.num_channels(), prediction.num_channels);
            check(equivalence, "Data",     logger, actual.span(),         prediction.data);
        }
    };
}
