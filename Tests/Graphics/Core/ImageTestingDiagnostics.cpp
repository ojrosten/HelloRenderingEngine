////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageTestingDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path image_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void image_false_negative_test::run_tests()
    {
        image red{working_materials() / "red_3_channels.png", vertically_flipped::no},
              striped{working_materials() / "striped_3_channels.png", vertically_flipped::no};

        auto almostRed{make_red(2, 3, 3)};
        almostRed.data.at(1) = 255;
        check(equivalence, "", red, almostRed);
        check(equality, "", red, striped);
    }
}
