////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/RegularTestCore.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

namespace sequoia::testing
{
    template<> struct value_tester<avocet::colour_channels>
    {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const avocet::colour_channels& actual, const avocet::colour_channels& prediction)
        {
            check(equality, "Raw value", logger, actual.raw_value(), prediction.raw_value());
        }
        
        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const avocet::colour_channels& actual, const std::size_t prediction)
        {
            check(equivalence, "Raw value", logger, actual.raw_value(), prediction);
        }
    };
}
