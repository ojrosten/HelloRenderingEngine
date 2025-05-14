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
            image{working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  all_channels_in_image},
            image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, all_channels_in_image},
            make_red(2, 3, image_channels{3}, 255),
            make_rgb_striped(2, 3, image_channels{3}),
            image_data{},
            image_data{}
        );

        check_semantics(
            "Override number of channels",
            image{working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  image_channels{1}},
            image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, image_channels{4}},
            make_red(2, 3, image_channels{1}, 76),
            make_rgb_striped(2, 3, image_channels{4}, 255),
            image_data{},
            image_data{}
        );

        check_semantics(
            "From aligned vector",
            image{make_red(2, 3, image_channels{3}, 255).data, 2, 3, image_channels{3}, alignment{1}},
            image{make_rgb_striped(2, 3, image_channels{4}, 0, alignment{4}).data, 2, 3, image_channels{4}, alignment{4}},
            make_red(2, 3, image_channels{3}, 255),
            make_rgb_striped(2, 3, image_channels{4}, 0, alignment{4}),
            image_data{},
            image_data{}
        );

        check_exception_thrown<std::runtime_error>(
            reporter{"Absent image"},
            [this](){
                return image{working_materials() / "Absent.png", flip_vertically::no, all_channels_in_image};
            },
            curlew::exception_postprocessor{}
        );

        check_exception_thrown<std::runtime_error>(
            reporter{"Invalid image"},
            [this](){
                return image{working_materials() / "not_an_image.txt", flip_vertically::no, all_channels_in_image};
            },
            curlew::exception_postprocessor{}
        );
    }
}
