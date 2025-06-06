////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "AlignmentTestingDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path alignment_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void alignment_false_negative_test::run_tests()
    {
        check(equivalence, "", alignment{}, 4uz);
        check(equality, "", alignment{4}, alignment{2});
    }
}
