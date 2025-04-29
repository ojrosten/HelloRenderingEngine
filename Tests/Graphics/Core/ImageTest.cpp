////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageTest.hpp"

#include <print>
#include <ranges>

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path image_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void image_test::run_tests()
    {
        check_exception_thrown<std::runtime_error>("File missing", [](){ return image{"absent.png", vertically_flipped::no}; });

        image red{working_materials() / "red_3_channels.png", vertically_flipped::no},
              striped{working_materials() / "striped_3_channels.png", vertically_flipped::no};

        check_semantics("",
                        image{working_materials() / "red_3_channels.png", vertically_flipped::no},
                        image{working_materials() / "striped_3_channels.png", vertically_flipped::no},
                        make_red(2, 3, 3),
                        make_striped(2, 3, 3));
    }
}
