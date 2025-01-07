////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/Graphics/OpenGL/Buffers.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class buffers_free_test final : public move_only_test
    {
    public:
        using move_only_test::move_only_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        void test_vbo();
        void test_vao();
        void test_ebo();

        template<class BufferObject, class T, std::size_t N>
        bool check_buffer_object(const reporter& description, const std::array<T, N>& buffer);
    };
}

namespace sequoia::testing
{
    template<>
    struct value_tester<avocet::opengl::vertex_buffer_object> {
        using type = avocet::opengl::vertex_buffer_object;

        template<test_mode Mode, class T>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const type& actual, const std::optional<std::vector<T>>& optPrediction)
        {
            avocet::opengl::gl_function{glBindBuffer}(GL_ARRAY_BUFFER, get_raw_index(actual));
            if(optPrediction) {
                const auto& prediction{optPrediction.value()};
                std::vector<T> recoveredBuffer(prediction.size());
                avocet::opengl::gl_function{glGetBufferSubData}(GL_ARRAY_BUFFER, 0, sizeof(T) * prediction.size(), recoveredBuffer.data());
                check(equality, "Buffer data", logger, recoveredBuffer, prediction);
            }
            else {
                check(equality, "Raw Index", logger, get_raw_index(actual), GLuint{});
            }
        }
    };
}
