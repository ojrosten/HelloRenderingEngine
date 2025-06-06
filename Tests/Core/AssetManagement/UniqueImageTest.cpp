////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "UniqueImageTest.hpp"
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
            unique_image{working_materials() / "red_2w_3h_3c.png",         flip_vertically::no},
            unique_image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes},
            make_red(        2, 3, colour_channels{3}),
            make_rgb_striped(2, 3, colour_channels{3}),
            image_data{},
            image_data{}
        );

        check_semantics(
            "",
            unique_image{make_red(        2, 3, colour_channels{3}).data, 2uz, 3uz, colour_channels{3}, alignment{1}},
            unique_image{make_rgb_striped(2, 3, colour_channels{3}).data, 2uz, 3uz, colour_channels{3}, alignment{1}},
            make_red(        2, 3, colour_channels{3}),
            make_rgb_striped(2, 3, colour_channels{3}),
            image_data{},
            image_data{}
        );

        check_exception_thrown<std::runtime_error>(
            "Absent image",
            [this](){
                return unique_image{working_materials() / "Absent.png", flip_vertically::no}; 
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Invalid image",
            [this](){
                return unique_image{working_materials() / "not_an_image.txt", flip_vertically::no};
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Inconsistent image data",
            []() {
                using value_t = unique_image::value_type;
                return unique_image{std::vector<value_t>{}, 2uz, 3uz, colour_channels{4}, alignment{1}};
            }
        );
    }
}
