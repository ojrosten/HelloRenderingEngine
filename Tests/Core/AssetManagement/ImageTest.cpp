////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageTest.hpp"
#include "curlew/TestFramework/GraphicsTestCore.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path image_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void image_test::run_tests()
    {
        check_semantics(
            "",
            image{working_materials() / "red_2w_3h_3c.png", flip_vertically::no},
            image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes},
            make_red(2, 3, 3),
            make_rgb_striped(2, 3, 3),
            image_data{},
            image_data{}
        );

        check_exception_thrown<std::runtime_error>(
            reporter{"Absent image"},
            [this](){
                return image{working_materials() / "Absent.png", flip_vertically::no}; 
            }
        );

        check_exception_thrown<std::runtime_error>(
            reporter{"Invalid image"},
            [this](){
                return image{working_materials() / "not_an_image.txt", flip_vertically::no};
            }
        );
    }
}
