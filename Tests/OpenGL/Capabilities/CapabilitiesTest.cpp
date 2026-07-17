////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CapabilitiesTest.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path capabilities_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void capabilities_test::run_tests()
    {
        check_exception_thrown<std::domain_error>(
            "",
            []() { return opengl::sample_coverage_value{-0.5f}; }
        );

        check_exception_thrown<std::domain_error>(
            "",
            []() { return opengl::sample_coverage_value{1.5f}; }
        );
    }
}
