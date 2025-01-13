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
    template<> struct value_tester<avocet::opengl::vertex_buffer_object>
    {
        using type = avocet::opengl::vertex_buffer_object;

        [[nodiscard]]
        static GLint get_buffer_size() {
            GLint param{};
            avocet::opengl::gl_function{glGetBufferParameteriv}(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &param);
            return param;
        }

        template<test_mode Mode, class T>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const std::optional<std::vector<T>>& prediction)
        {
            bind(actual);

            if(prediction) {
                std::vector<T> recoveredBuffer(get_buffer_size() / sizeof(T));
                avocet::opengl::gl_function{glGetBufferSubData}(GL_ARRAY_BUFFER, 0, sizeof(T) * prediction->size(), recoveredBuffer.data());
                check(equality, "Buffer data", logger, recoveredBuffer, *prediction);
            }
            else {
                check("Null buffer", logger, static_cast<bool>(actual));
            }
        }
    };
}
