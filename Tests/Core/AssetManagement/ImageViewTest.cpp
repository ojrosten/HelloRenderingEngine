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
        check_semantics(
            "",
            image_view{to_unique_image(make_red(2, 3, 4))},
            image_view{to_unique_image(make_red(2, 3, 3))},
            make_red(2, 3, 4),
            make_red(2, 3, 3)
        );
    }
}
