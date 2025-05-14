////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

#include <ranges>

namespace avocet::testing {
    struct image_data {
        using value_type = unsigned char;

        std::vector<value_type> data;
        std::size_t width{}, height{};
        image_channels num_channels{};
        alignment row_alignment{};
    };

    [[nodiscard]]
    image_data make_red(std::size_t width, std::size_t height, image_channels channels, std::size_t intensity);

    [[nodiscard]]
    image_data make_rgb_striped(std::size_t w, std::size_t h, image_channels channels);
}

namespace sequoia::testing
{
    template<> struct value_tester<avocet::image>
    {
        using type       = avocet::image;
        using image_data = avocet::testing::image_data;
        using value_type = image_data::value_type;

        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const type& actual, const type& prediction)
        {
            check(equality, "Width",     logger, actual.width(),         prediction.width());
            check(equality, "Height",    logger, actual.height(),        prediction.height());
            check(equality, "Channels",  logger, actual.num_channels(),  prediction.num_channels());
            check(equality, "Alignment", logger, actual.row_alignment(), prediction.row_alignment());
            check(equality, "Data",      logger, actual.span(),          prediction.span());
        }

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const image_data& prediction)
        {
            check(equality,    "Width",     logger, actual.width(),         prediction.width);
            check(equality,    "Height",    logger, actual.height(),        prediction.height);
            check(equality,    "Channels",  logger, actual.num_channels(),  prediction.num_channels);
            check(equality,    "Alignment", logger, actual.row_alignment(), prediction.row_alignment);
            check(equivalence, "Data",      logger, actual.span(),          prediction.data);
        }
    };
}
