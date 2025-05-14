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
        check_padded_row_size();
        check_image();
    }

    void image_test::check_padded_row_size() {
        check(equality, "", padded_row_size(0, image_channels{1}, alignment{1}), std::size_t{});
        check(equality, "", padded_row_size(1, image_channels{1}, alignment{1}), std::size_t{1});
        check(equality, "", padded_row_size(1, image_channels{1}, alignment{2}), std::size_t{2});
        check(equality, "", padded_row_size(2, image_channels{1}, alignment{1}), std::size_t{2});
        check(equality, "", padded_row_size(2, image_channels{1}, alignment{2}), std::size_t{2});
        check(equality, "", padded_row_size(2, image_channels{1}, alignment{4}), std::size_t{4});

        check(equality, "", padded_row_size(1, image_channels{2}, alignment{1}), std::size_t{2});
        check(equality, "", padded_row_size(1, image_channels{2}, alignment{2}), std::size_t{2});
        check(equality, "", padded_row_size(1, image_channels{2}, alignment{4}), std::size_t{4});
        check(equality, "", padded_row_size(2, image_channels{2}, alignment{2}), std::size_t{4});
        check(equality, "", padded_row_size(2, image_channels{2}, alignment{4}), std::size_t{4});

        check(equality, "", padded_row_size(3, image_channels{2}, alignment{4}), std::size_t{8});
    }

    void image_test::check_image() {

        check_semantics(
            "",
            image{working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  all_channels_in_image},
            image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, all_channels_in_image},
            make_red(2, 3, image_channels{3}, alignment{1}, 255),
            make_rgb_striped(2, 3, image_channels{3}, alignment{1}),
            image_data{},
            image_data{}
        );

        check_semantics(
            "Override number of channels",
            image{working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  image_channels{1}},
            image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, image_channels{4}},
            make_red(2, 3, image_channels{1}, alignment{1}, 76),
            make_rgb_striped(2, 3, image_channels{4}, alignment{1}, 255),
            image_data{},
            image_data{}
        );

        check_semantics(
            "From vector with aligned rows",
            to_image(make_red(        2, 3, image_channels{3}, alignment{1}, 255)),
            to_image(make_rgb_striped(2, 3, image_channels{4}, alignment{4})),
            make_red(        2, 3, image_channels{3}, alignment{1}, 255),
            make_rgb_striped(2, 3, image_channels{4}, alignment{4}),
            image_data{},
            image_data{}
        );

        check_semantics(
            "From vector with padded rows",
            to_image(make_red(2, 3, image_channels{3}, alignment{4}, 255)),
            to_image(make_red(2, 3, image_channels{1}, alignment{2}, 0)),
            make_red(2, 3, image_channels{3}, alignment{4}, 255),
            make_red(2, 3, image_channels{1}, alignment{2}, 0),
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
