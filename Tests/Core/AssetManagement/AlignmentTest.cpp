////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "AlignmentTest.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path alignment_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void alignment_test::run_tests()
    {
        check_exception_thrown<std::runtime_error>(
            "Alignment of zero",
            []() { return alignment{0}; }
        );

        check_exception_thrown<std::runtime_error>(
            "Alignment that is an odd number, not equal to 1",
            []() { return alignment{3}; }
        );

        check_exception_thrown<std::runtime_error>(
            "Alignment that is an even number which is not a non-negative power of 2",
            []() { return alignment{14}; }
        );

        check_semantics("", alignment{}, alignment{4}, 1uz, 4uz, std::strong_ordering::less);
    }
}
