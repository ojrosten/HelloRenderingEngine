////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "ColourChannelsTestingUtilities.hpp"
#include "AlignmentTestingUtilities.hpp"

#include "sequoia/TestFramework/FreeCheckers.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

#include <ranges>

namespace avocet::testing {
    struct image_data {
        using value_type = unsigned char;

        std::vector<value_type> data;
        std::size_t width{}, height{};
        colour_channels num_channels{};
        alignment row_alignment{};
    };

    [[nodiscard]]
    inline unique_image to_unique_image(const image_data& im) {
        return {im.data, im.width, im.height, im.num_channels, im.row_alignment};
    }

    [[nodiscard]]
    image_data make_red(std::size_t width, std::size_t height, colour_channels channels, alignment rowAlignment);

    [[nodiscard]]
    image_data make_rgb_striped(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment);
}

namespace sequoia::testing
{
    template<class T>
    struct image_value_tester
    {
        using image_data = avocet::testing::image_data;

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const T& actual, const image_data& prediction)
        {
            check(equality,    "Width",     logger, actual.width(),         prediction.width);
            check(equality,    "Height",    logger, actual.height(),        prediction.height);
            check(equality,    "Channels",  logger, actual.num_channels(),  prediction.num_channels);
            check(equality,    "Alignment", logger, actual.row_alignment(), prediction.row_alignment);
            check(equivalence, "Data",      logger, actual.span(),          prediction.data);
        }
    protected:
        template<test_mode Mode>
        static void do_test(equality_check_t, test_logger<Mode>& logger, const T& actual, const T& prediction)
        {
            check(equality, "Width",     logger, actual.width(),         prediction.width());
            check(equality, "Height",    logger, actual.height(),        prediction.height());
            check(equality, "Channels",  logger, actual.num_channels(),  prediction.num_channels());
            check(equality, "Alignment", logger, actual.row_alignment(), prediction.row_alignment());
        }
    };

    template<> struct value_tester<avocet::unique_image> : image_value_tester<avocet::unique_image>
    {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const avocet::unique_image& actual, const avocet::unique_image& prediction)
        {
            image_value_tester<avocet::unique_image>::do_test(equality, logger, actual, prediction);
            check(equality, "Data Handle", logger, actual.span().data(), prediction.span().data());
            check(equality, "Data Size",   logger, actual.span().size(), prediction.span().size());
        }

        using image_value_tester<avocet::unique_image>::test;
    };

    template<> struct value_tester<avocet::image_view> : image_value_tester<avocet::image_view> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const avocet::image_view& actual, const avocet::image_view& prediction)
        {
            image_value_tester<avocet::image_view>::do_test(equality, logger, actual, prediction);
            check(equality, "Data", logger, actual.span(), prediction.span());
        }

        using image_value_tester<avocet::image_view>::test;
    };
}
