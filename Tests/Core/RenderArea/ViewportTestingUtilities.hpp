////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/RegularTestCore.hpp"
#include "avocet/Core/RenderArea/Viewport.hpp"

namespace sequoia::testing
{
    template<> struct value_tester<avocet::discrete_extent>
    {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const avocet::discrete_extent& actual, const avocet::discrete_extent& prediction)
        {
            check(equality, "Width",  logger, actual.width, prediction.width);
            check(equality, "Height", logger, actual.height, prediction.height);
        }
    };

    template<> struct value_tester<avocet::discrete_offset>
    {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const avocet::discrete_offset& actual, const avocet::discrete_offset& prediction)
        {
            check(equality, "x", logger, actual.x, prediction.x);
            check(equality, "y", logger, actual.y, prediction.y);
        }
    };

    template<> struct value_tester<avocet::viewport>
    {
        using type = avocet::viewport;

        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const type& actual, const type& prediction)
        {
            check(equality, "Offset", logger, actual.offset, prediction.offset);
            check(equality, "Extent", logger, actual.extent, prediction.extent);
        }
    };
}
