////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ResourceHandleTestingDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path resource_handle_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void resource_handle_false_negative_test::run_tests()
    {
        using rh = avocet::opengl::resource_handle;

        check(equivalence, "Wrong wrapped value", rh{43}, 42);
    }
}
