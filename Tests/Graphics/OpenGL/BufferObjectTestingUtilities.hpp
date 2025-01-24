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
    namespace agl = avocet::opengl;

    template<agl::buffer_species Species, class T>
    struct generic_buffer_object_tester
    {
        using type = agl::generic_buffer_object<Species, T>;

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const std::optional<std::vector<T>>& prediction)
        {
            if(prediction) {
                const auto recoveredBuffer{extract_buffer_sub_data(actual)};
                check(equality, "Buffer data", logger, recoveredBuffer, *prediction);
            }
            else {
                check("Null buffer", logger, static_cast<bool>(actual));
            }
        }
    };

    template<class T> struct value_tester<agl::vertex_buffer_object<T>> : generic_buffer_object_tester<agl::buffer_species::array, T>
    {};

    template<class T> struct value_tester<agl::element_buffer_object<T>> : generic_buffer_object_tester<agl::buffer_species::element_array, T>
    {
    };

    template<class B>
    inline constexpr bool is_gl_buffer_v{
        requires {
            typename B::value_type;
            requires std::is_same_v<B, agl::vertex_buffer_object<typename B::value_type>> || std::is_same_v<B, agl::element_buffer_object<typename B::value_type>>;
        }
    };
}
