////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/OpenGL/Graphics/ResourceHandle.hpp"

namespace sequoia::testing
{
    template<> struct value_tester<avocet::opengl::resource_handle> {
        using rh = avocet::opengl::resource_handle;

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const rh& actual, GLuint prediction) {
            check(equality, "Wrapped Value", logger, actual.index(), prediction);
        }
    };
}
