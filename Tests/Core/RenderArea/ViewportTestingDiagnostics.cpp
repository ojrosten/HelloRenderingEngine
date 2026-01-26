////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ViewportTestingDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path viewport_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void viewport_false_negative_test::run_tests()
    {
        avocet::viewport x{{}, {800, 600}}, y{{42, 1729}, {5, 7}};
        check(equality, "", x, y);
    }
}
