////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BufferMetaFreeTest.hpp"
#include "avocet/OpenGL/Graphics/Buffers.hpp"

namespace
{
    struct foo{};
}

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path buffer_meta_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffer_meta_free_test::run_tests()
    {
        using namespace opengl;

        STATIC_CHECK(not has_gl_arithmetic_type_of_v<foo>);

        STATIC_CHECK(has_gl_arithmetic_type_of_v<float>);

        STATIC_CHECK(has_gl_arithmetic_type_of_v<std::array<float, 2>>);

        STATIC_CHECK(not has_gl_arithmetic_type_of_v<std::vector<foo>>);

        STATIC_CHECK(has_gl_arithmetic_type_of_v<std::array<std::array<float, 2>, 3>>);
    }
}
