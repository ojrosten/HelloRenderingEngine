////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BufferMetaFreeTest.hpp"
#include "avocet/OpenGL/Graphics/Buffers.hpp"

namespace avocet::testing
{
    namespace
    {
        struct foo {};

        template<class T>
        constexpr bool has_gl_arithmetic_type_of_v{
            requires { typename opengl::gl_arithmetic_type_of_t<T>; }
        };
    }

    [[nodiscard]]
    std::filesystem::path buffer_meta_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_meta_free_test::run_tests()
    {
        check_arithmetic_type_of();
        check_legal_buffer_value_type();
    }

    void buffer_meta_free_test::check_arithmetic_type_of()
    {
        STATIC_CHECK(not has_gl_arithmetic_type_of_v<foo>);
        STATIC_CHECK(    has_gl_arithmetic_type_of_v<float>);

        STATIC_CHECK(not has_gl_arithmetic_type_of_v<std::vector<foo>>);
        STATIC_CHECK(    has_gl_arithmetic_type_of_v<std::vector<float>>);

        STATIC_CHECK(not has_gl_arithmetic_type_of_v<std::vector<std::vector<foo>>>);
        STATIC_CHECK(    has_gl_arithmetic_type_of_v<std::vector<std::vector<float>>>);
    }

    void buffer_meta_free_test::check_legal_buffer_value_type()
    {
        using namespace opengl;

        STATIC_CHECK(not is_legal_gl_buffer_value_type_v<foo>);
        STATIC_CHECK(    is_legal_gl_buffer_value_type_v<float>);

        STATIC_CHECK(std::same_as<is_legal_gl_buffer_value_type_t<foo>,   std::false_type>);
        STATIC_CHECK(std::same_as<is_legal_gl_buffer_value_type_t<float>, std::true_type>);

        STATIC_CHECK(not is_legal_gl_buffer_value_type_v<sequoia::mem_ordered_tuple<foo>>);
        STATIC_CHECK(    is_legal_gl_buffer_value_type_v<sequoia::mem_ordered_tuple<float>>);
    }
}
