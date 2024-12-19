////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ResourceHandleTest.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path resource_handle_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void resource_handle_test::run_tests()
    {
        auto x = []() { return avocet::opengl::resource_handle{42}; };
        auto y = []() { return avocet::opengl::resource_handle{1729}; };
        check(equivalence, "", x(), 42);
        check(equivalence, "", y(), 1729);

        check_semantics("", x, y);
    }
}
