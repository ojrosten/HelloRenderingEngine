////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "LabellingTestDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path labelling_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void labelling_false_negative_test::labelling_tests()
    {
    }

    [[nodiscard]]
    std::filesystem::path labelling_false_positive_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void labelling_false_positive_test::labelling_tests()
    {
    }
}
