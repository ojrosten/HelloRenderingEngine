////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageViewTestingDiagnostics.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path image_view_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void image_view_false_negative_test::run_tests()
    {
        unique_image red{make_red(3, 2, colour_channels{4}).data, 3, 2, colour_channels{4}, alignment{1}};
        check(equivalence, "", image_view{red}, make_red(4, 3, colour_channels{2}));
        check(equality,    "", image_view{red}, image_view{to_unique_image(make_rgb_striped(3, 2, colour_channels{4}))});
    }
}
