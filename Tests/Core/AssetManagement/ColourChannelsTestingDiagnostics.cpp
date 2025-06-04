////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ColourChannelsTestingDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path colour_channels_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void colour_channels_false_negative_test::run_tests()
    {
        check(equivalence, "", colour_channels{}, 4uz);
        check(equality, "", colour_channels{3}, colour_channels{2});
    }
}
