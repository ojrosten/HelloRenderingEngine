////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/Graphics/OpenGL/Buffers.hpp"

namespace sequoia::testing
{
    template<class T> struct value_tester<avocet::opengl::vertex_buffer_object<T>>
    {
        using type = avocet::opengl::vertex_buffer_object<T>;

        template<test_mode Mode, class T>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const std::optional<std::vector<T>>& prediction)
        {
            if(prediction) {
                const auto recoveredBuffer{actual.get_buffer_sub_data()};
                check(equality, "Buffer data", logger, recoveredBuffer, *prediction);
            }
            else {
                check("Null buffer", logger, static_cast<bool>(actual));
            }
        }
    };
}
