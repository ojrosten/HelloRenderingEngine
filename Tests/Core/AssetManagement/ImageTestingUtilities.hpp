////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "avocet/Core/AssetManagement/Image.hpp"

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "sequoia/TestFramework/RegularTestCore.hpp"

namespace avocet::testing {
    struct image_data {
        using value_type = unsigned char;

        std::vector<value_type> data;
        std::size_t width{}, height{};
        colour_channels num_channels{};
        alignment row_alignment{};
    };

    [[nodiscard]]
    unique_image to_image(image_data imageData);

    [[nodiscard]]
    image_data make_red(std::size_t width, std::size_t height, colour_channels channels, alignment rowAlignment, unsigned char intensity);

    [[nodiscard]]
    image_data make_rgb_striped(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, unsigned char alpha=0);
 
    template<sequoia::testing::concrete_test Test, class Transform>
    void execute_image_false_negative_tests(Test& test, Transform transform) {
        using namespace sequoia::testing;
        unique_image red{test.working_materials() / "red_2w_3h_3c.png", flip_vertically::no, all_channels_in_image};

        test.check(equivalence, "Wrong unique_image", transform(red), make_red(        3, 2, colour_channels{4}, alignment{1}, 255));
        test.check(equivalence, "Wrong colours",      transform(red), make_rgb_striped(2, 3, colour_channels{3}, alignment{1}));

        test.check(equality, "Wrong unique_image",   transform(red), transform(unique_image{test.working_materials() / "grey_3w_2h_1c.png", flip_vertically::no, all_channels_in_image}));
        test.check(equality, "Wrong colours",        transform(red), transform(unique_image{test.working_materials() / "blue_2w_3h_3c.png", flip_vertically::no, all_channels_in_image}));

        test.check(equality, "Not padded", transform(to_image(make_red(        2, 3, colour_channels{3}, alignment{1}, 255))), transform(to_image(make_red(        2, 3, colour_channels{3}, alignment{4}, 255))));
        test.check(equality, "Not padded", transform(to_image(make_rgb_striped(1, 1, colour_channels{4}, alignment{1}, 255))), transform(to_image(make_rgb_striped(1, 1, colour_channels{3}, alignment{4}, 255))));
    }

    template<sequoia::testing::concrete_test Test, class Transform, class... Args>
    void execute_image_tests(Test& test, Transform transform, Args... args) {
        using namespace sequoia::testing;
        test.template check_exception_thrown<std::runtime_error>(
            reporter{"Absent unique_image"},
            [&test, transform](){
                return transform(unique_image{unique_image{test.working_materials() / "Absent.png", flip_vertically::no, all_channels_in_image}});
            }
        );

        test.template check_exception_thrown<std::runtime_error>(
            reporter{"Invalid unique_image"},
            [&test, transform](){
                return transform(unique_image{unique_image{test.working_materials() / "not_an_image.txt", flip_vertically::no, all_channels_in_image}});
            }
        );

        test.check_semantics(
            "",
            transform(unique_image{unique_image{test.working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  all_channels_in_image}}),
            transform(unique_image{unique_image{test.working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, all_channels_in_image}}),
            make_red(2, 3, colour_channels{3}, alignment{1}, 255),
            make_rgb_striped(2, 3, colour_channels{3}, alignment{1}),
            args...
        );

        test.check_semantics(
            "Override number of channels",
            transform(unique_image{unique_image{test.working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  colour_channels{1}}}),
            transform(unique_image{unique_image{test.working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, colour_channels{4}}}),
            make_red(2, 3, colour_channels{1}, alignment{1}, 76),
            make_rgb_striped(2, 3, colour_channels{4}, alignment{1}, 255),
            args...
        );

        test.check_semantics(
            "From vector with aligned rows",
            transform(unique_image{to_image(make_red(        2, 3, colour_channels{3}, alignment{1}, 255))}),
            transform(unique_image{to_image(make_rgb_striped(2, 3, colour_channels{4}, alignment{4}))}),
            make_red(2, 3, colour_channels{3}, alignment{1}, 255),
            make_rgb_striped(2, 3, colour_channels{4}, alignment{4}),
            args...
        );

        test.check_semantics(
            "From vector with padded rows",
            transform(unique_image{to_image(make_red(2, 3, colour_channels{3}, alignment{4}, 255))}),
            transform(unique_image{to_image(make_red(2, 3, colour_channels{1}, alignment{2}, 0))}),
            make_red(2, 3, colour_channels{3}, alignment{4}, 255),
            make_red(2, 3, colour_channels{1}, alignment{2}, 0),
            args...
        );
    }
}

namespace sequoia::testing
{
    template<class T>
    struct image_value_tester 
    {
        using image_data = avocet::testing::image_data;

        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const T& actual, const T& prediction)
        {
            check(equality, "Width",     logger, actual.width(),         prediction.width());
            check(equality, "Height",    logger, actual.height(),        prediction.height());
            check(equality, "Channels",  logger, actual.num_channels(),  prediction.num_channels());
            check(equality, "Alignment", logger, actual.row_alignment(), prediction.row_alignment());
            check(equality, "Data",      logger, actual.span(),          prediction.span());
        }

        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const T& actual, const image_data& prediction)
        {
            check(equality,    "Width",     logger, actual.width(),         prediction.width);
            check(equality,    "Height",    logger, actual.height(),        prediction.height);
            check(equality,    "Channels",  logger, actual.num_channels(),  prediction.num_channels);
            check(equality,    "Alignment", logger, actual.row_alignment(), prediction.row_alignment);
            check(equivalence, "Data",      logger, actual.span(),          prediction.data);
        }
    };

    template<> struct value_tester<avocet::unique_image> : image_value_tester<avocet::unique_image> {};

    template<> struct value_tester<avocet::image_view> : image_value_tester<avocet::image_view> {};
}
