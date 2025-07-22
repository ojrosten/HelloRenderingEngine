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
    }

    void buffer_meta_free_test::check_legal_buffer_value_type()
    {
    }
}
