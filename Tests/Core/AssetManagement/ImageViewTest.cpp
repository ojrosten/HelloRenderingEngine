////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageViewTest.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path image_view_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void image_view_test::run_tests()
    {
        check_semantics_via_image_data(
            "",
            make_red(        1, 3, colour_channels{2}, alignment{1}),
            make_rgb_striped(3, 2, colour_channels{4}, alignment{1})
        );
    }
}
