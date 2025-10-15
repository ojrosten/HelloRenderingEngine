////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/OpenGL/Resources/Buffers.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

namespace sequoia::testing
{
    namespace agl = avocet::opengl;

    struct generic_buffer_object_tester {
        template<test_mode Mode, agl::buffer_species Species, agl::gl_arithmetic T>
        static void test(equivalence_check_t,
                         test_logger<Mode>& logger,
                         const agl::generic_buffer_object<Species, T>& buffer,
                         const std::optional<std::span<const T>>& prediction)
        {
            if(prediction) {
                check(equality,
                    "Buffer Data",
                    logger,
                    std::span<const T>{buffer.extract_data()},
                    prediction.value());
            }
            else {
                check("Null Buffer", logger, buffer.is_null());
            }
        }
    };

    template<agl::gl_arithmetic T>
    struct value_tester<agl::vertex_buffer_object<T>> : generic_buffer_object_tester {};

    template<agl::gl_arithmetic T>
    struct value_tester<agl::element_buffer_object<T>> : generic_buffer_object_tester {};

    template<class B>
    inline constexpr bool is_gl_buffer_v{
        requires {
            typename B::value_type;
            requires std::is_same_v<B, agl::vertex_buffer_object<typename B::value_type>> || std::is_same_v<B, agl::element_buffer_object<typename B::value_type>>;
        }
    };
}
