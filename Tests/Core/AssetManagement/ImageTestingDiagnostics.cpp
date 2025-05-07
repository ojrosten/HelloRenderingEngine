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
        image red{working_materials() / "red_2w_3h_3c.png", flip_vertically::no};

        check(equivalence, "Wrong image",   red, make_red(3, 2, 4));
        check(equivalence, "Wrong colours", red, make_rgb_striped(2, 3, 3));

        check(equality, "Wrong image",   red, image{working_materials() / "grey_3w_2h_1c.png", flip_vertically::no});
        check(equality, "Wrong colours", red, image{working_materials() / "blue_2w_3h_3c.png", flip_vertically::no});
    }
}
