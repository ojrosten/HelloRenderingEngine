////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "UniqueImageTestingDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path image_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void image_false_negative_test::run_tests()
    {
        unique_image red{working_materials() / "red_2w_3h_3c.png", flip_vertically::no, all_channels_in_image};

        check(equivalence, "Wrong image",     red, make_red(        3, 2, colour_channels{4}, alignment{1}));
        check(equivalence, "Wrong colours",   red, make_rgb_striped(2, 3, colour_channels{3}, alignment{1}));
        check(equivalence, "Wrong alignment", red, make_red(        2, 3, colour_channels{3}, alignment{2}));

        check(equivalence,
              "Padding where there should be an alpha channel",
              to_unique_image(make_red(1, 1, colour_channels{1}, alignment{2}, monochrome_intensity{.red{255}, .alpha{42}})),
                              make_red(1, 1, colour_channels{2}, alignment{1}, monochrome_intensity{.red{255}, .alpha{42}}));

        check(equality, "Wrong image",     red, unique_image{working_materials() / "grey_3w_2h_1c.png", flip_vertically::no, all_channels_in_image});
        check(equality, "Wrong colours",   red, unique_image{working_materials() / "blue_2w_3h_3c.png", flip_vertically::no, all_channels_in_image});
        check(equality, "Wrong alignment", red, to_unique_image(make_red(2, 3, colour_channels{3}, alignment{2})));
    }
}
