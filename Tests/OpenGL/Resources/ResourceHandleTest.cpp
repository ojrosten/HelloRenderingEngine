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
        using rh = avocet::opengl::resource_handle;
        check_semantics("", rh{42}, rh{1729}, 42, 1729, 0, 42);
    }
}
