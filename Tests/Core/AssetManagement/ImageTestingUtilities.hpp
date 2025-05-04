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

        std::size_t width{}, height{}, num_channels{};
        std::vector<value_type> data;
    };

    [[nodiscard]]
    image_data make_red(std::size_t width, std::size_t height, std::size_t channels);

    [[nodiscard]]
    image_data make_striped(std::size_t w, std::size_t h, std::size_t channels);
}

namespace sequoia::testing
{
    template<> struct value_tester<avocet::image>
    {
        using type       = avocet::image;
        using image_data = avocet::testing::image_data;
        using value_type = image_data::value_type;

        // This is used to convert a vector of unsigned chars to a vector of unsigned ints.
        // The transform is done to make the output of failed tests more comprehensible.
        // Fpor example a colour of 0 or 255 is much more comprehensible as an int that an
        // ASCII character.
        // A new vector is materialized to make the output for failed tests platform-independent.
        // (Otherwise the output depends on details of transform_view.)
        // I will investigate making improvements to sequoia to obviate the need for the this.
        static auto as_unsigned_int(std::span<const value_type> data) {
            return std::views::transform(data, [](auto c) -> unsigned int { return c; }) | std::ranges::to<std::vector>();
        }

        /*template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const type& actual, const type& prediction)
        {
            check(equality, "Width",    logger, actual.width(),                 prediction.width());
            check(equality, "Height",   logger, actual.height(),                prediction.height());
            check(equality, "Channels", logger, actual.num_channels(),          prediction.num_channels());
            check(equality, "Data",     logger, as_unsigned_int(actual.span()), as_unsigned_int(prediction.span()));
        }

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const image_data& prediction)
        {
            check(equality,    "Width",    logger, actual.width(),                 prediction.width);
            check(equality,    "Height",   logger, actual.height(),                prediction.height);
            check(equality,    "Channels", logger, actual.num_channels(),          prediction.num_channels);
            check(equivalence, "Data",     logger, as_unsigned_int(actual.span()), as_unsigned_int(prediction.data));
        }*/
    };
}
