////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/RegularTestCore.hpp"
#include "avocet/Graphics/Core/Image.hpp"

namespace sequoia::testing
{
    template<> struct value_tester<avocet::image>
    {
        using type = avocet::image;
        struct image_data {
            std::size_t width{}, height{}, num_channels{};
            std::span<unsigned char> span;
        };

        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const type& actual, const type& prediction)
        {
            check(equality, "Width",    logger, actual.width(),        prediction.width());
            check(equality, "Height",   logger, actual.height(),       prediction.height());
            check(equality, "Channels", logger, actual.num_channels(), prediction.num_channels());
            check(equality, "Data",     logger, actual.span(),         prediction.span());
        }

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const image_data& prediction)
        {
            check(equality, "Width",    logger, actual.width(),        prediction.width);
            check(equality, "Height",   logger, actual.height(),       prediction.height);
            check(equality, "Channels", logger, actual.num_channels(), prediction.num_channels);
            check(equality, "Data",     logger, actual.span(),         prediction.span);
        }
    };
}
