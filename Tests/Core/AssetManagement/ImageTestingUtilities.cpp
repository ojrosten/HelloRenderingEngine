////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageTestingUtilities.hpp"
#include "ImageTest.hpp"
#include "ImageTestingDiagnostics.hpp"
#include "ImageViewTest.hpp"
#include "ImageViewTestingDiagnostics.hpp"

#include <ranges>

namespace avocet::testing {
    namespace {
        [[nodiscard]]
        unique_image to_image(image_data imageData) {
            return {imageData.data, imageData.width, imageData.height, imageData.num_channels, imageData.row_alignment};
        }

        template<class Fn>
          requires std::is_invocable_r_v<typename image_data::value_type, Fn, std::size_t, std::size_t>
        [[nodiscard]]
        image_data make_image(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, Fn fn) {
            constexpr auto bytesPerChannel{sizeof(image_data::value_type)};
            const auto paddedRowSize{padded_row_size(w, channels, rowAlignment, bytesPerChannel)};
            const bool isPadded{paddedRowSize != padded_row_size(w, channels, alignment{1}, bytesPerChannel)};
            return {
                .data{
                      std::views::iota(0u, paddedRowSize * h)
                    | std::views::transform(
                        [=](auto i) -> unsigned char {
                            if(const bool paddingByte{isPadded && !((i + 1) % paddedRowSize)}; paddingByte)
                                return 0;

                            const auto row{i / paddedRowSize};
                            const auto channelIndex{(i - paddedRowSize * row) % channels.raw_value()};
                            return fn(row, channelIndex);
                        }
                      )
                    | std::ranges::to<std::vector>()
                },
                .width{w},
                .height{h},
                .num_channels{channels},
                .row_alignment{rowAlignment}
            };
        }

        [[nodiscard]]
        image_data make_red(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, unsigned char intensity) {
            return make_image(w, h, channels, rowAlignment, [intensity](std::size_t, std::size_t channelIndex){ return static_cast<unsigned char>(channelIndex ? 0 : intensity); });
        }

        [[nodiscard]]
        image_data make_rgb_striped(std::size_t w, std::size_t h, colour_channels channels, alignment rowAlignment, unsigned char alpha = 0) {
            const auto fn{
                [channels, alpha](std::size_t row, std::size_t channelIndex){
                    if(channelIndex == 3)
                         return alpha;

                    return static_cast<unsigned char>((row % channels.raw_value()) == channelIndex ? 255 : 0);
                }
            };
            return make_image(w, h, channels, rowAlignment, fn);
        };
    }

    template void execute_image_false_negative_tests(image_false_negative_test&, std::identity);
    template void execute_image_false_negative_tests(image_view_false_negative_test&, to_image_view);

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

    template void execute_image_tests(image_test&, std::identity, image_data, image_data);
    template void execute_image_tests(image_view_test&, to_image_view);

    template<sequoia::testing::concrete_test Test, class Transform, class... Args>
    void execute_image_tests(Test& test, Transform transform, Args... args) {
        using namespace sequoia::testing;
        test.template check_exception_thrown<std::runtime_error>(
            reporter{"Absent unique_image"},
            [&test, transform](){
                return transform(unique_image{test.working_materials() / "Absent.png", flip_vertically::no, all_channels_in_image});
            }
        );

        test.template check_exception_thrown<std::runtime_error>(
            reporter{"Invalid unique_image"},
            [&test, transform](){
                return transform(unique_image{test.working_materials() / "not_an_image.txt", flip_vertically::no, all_channels_in_image});
            }
        );

        test.check_semantics(
            "",
            transform(unique_image{test.working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  all_channels_in_image}),
            transform(unique_image{test.working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, all_channels_in_image}),
            make_red(2, 3, colour_channels{3}, alignment{1}, 255),
            make_rgb_striped(2, 3, colour_channels{3}, alignment{1}),
            args...
        );

        test.check_semantics(
            "Override number of channels",
            transform(unique_image{test.working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  colour_channels{1}}),
            transform(unique_image{test.working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, colour_channels{4}}),
            make_red(2, 3, colour_channels{1}, alignment{1}, 76),
            make_rgb_striped(2, 3, colour_channels{4}, alignment{1}, 255),
            args...
        );

        test.check_semantics(
            "From vector with aligned rows",
            transform(to_image(make_red(        2, 3, colour_channels{3}, alignment{1}, 255))),
            transform(to_image(make_rgb_striped(2, 3, colour_channels{4}, alignment{4}))),
            make_red(2, 3, colour_channels{3}, alignment{1}, 255),
            make_rgb_striped(2, 3, colour_channels{4}, alignment{4}),
            args...
        );

        test.check_semantics(
            "From vector with padded rows",
            transform(to_image(make_red(2, 3, colour_channels{3}, alignment{4}, 255))),
            transform(to_image(make_red(2, 3, colour_channels{1}, alignment{2}, 0))),
            make_red(2, 3, colour_channels{3}, alignment{4}, 255),
            make_red(2, 3, colour_channels{1}, alignment{2}, 0),
            args...
        );
    }
}
