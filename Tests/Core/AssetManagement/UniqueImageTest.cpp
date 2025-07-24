////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "UniqueImageTest.hpp"
#include "curlew/TestFramework/GraphicsTestCore.hpp"

#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"

#include <future>
#include <latch>
#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path unique_image_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void unique_image_test::run_tests()
    {
        check_exception_thrown<std::runtime_error>(
            "Absent image",
            [this](){
                return unique_image{working_materials() / "Absent.png", flip_vertically::no, all_channels_in_image};
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Invalid image",
            [this](){
                return unique_image{working_materials() / "not_an_image.txt", flip_vertically::no, all_channels_in_image};
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Inconsistent image data",
            []() {
                using value_t = unique_image::value_type;
                return unique_image{std::vector<value_t>{}, 2uz, 3uz, colour_channels{4}, alignment{1}};
            }
        );

        check_semantics(
            "",
            unique_image{working_materials() / "red_2w_3h_3c.png",         flip_vertically::no,  all_channels_in_image},
            unique_image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, all_channels_in_image},
            make_red(        2, 3, colour_channels{3}, alignment{1}),
            make_rgb_striped(2, 3, colour_channels{3}, alignment{1}),
            image_data{},
            image_data{}
        );

        check_semantics_via_image_data(
            "",
            make_red(        2, 3, colour_channels{3}, alignment{1}),
            make_rgb_striped(2, 3, colour_channels{3}, alignment{1})
        );

        check_semantics_via_image_data(
            "",
            make_red(1, 1, colour_channels{2}, alignment{1}, monochrome_intensity{.red{255}, .alpha{42}}),
            make_red(1, 1, colour_channels{2}, alignment{2}, monochrome_intensity{.red{255}, .alpha{42}})
        );

        check(
            equivalence,
            "Override number of channels: the three RGB channels are reduced to grey scale by stb using the formula ((r*77) + (g*150) +  (29*b)) >> 8"
            "The coefficients approximate the human eye's sensitivity to different frequencies",
            unique_image{working_materials() / "red_2w_3h_3c.png", flip_vertically::no, colour_channels{1}},
            make_red(2, 3, colour_channels{1}, alignment{1}, monochrome_intensity{.red{76}, .alpha{0}})
        );

        check(
            equivalence,
            "Override number of channels: the three RGB channels are reduced two channels - grey scale computed as above plus alpha set to 255",
            unique_image{working_materials() / "red_2w_3h_3c.png", flip_vertically::no, colour_channels{2}},
            make_red(2, 3, colour_channels{2}, alignment{1}, monochrome_intensity{.red{76}, .alpha{255}})
        );

        check(
            equivalence,
            "Override number of channels: three RGB channels expands to RGBA with alpha set to 255",
            unique_image{working_materials() / "red_2w_3h_3c.png", flip_vertically::no, colour_channels{4}},
            make_red(2, 3, colour_channels{4}, alignment{1}, monochrome_intensity{.red{255}, .alpha{255}})
        );

        //const auto data = make_bgr_striped(100, 3, colour_channels{3}, alignment{1});
        //stbi_write_png((working_materials() / "bgr_striped_100w_3h_3c.png").generic_string().data(), 100, 3, 3, data.data.data(), 100 * 3);

        constexpr std::size_t numThreads{6};
        using promise_t = std::promise<unique_image>;
        using future_t  = std::future<unique_image>;
        std::array<promise_t, numThreads> imagePromises{};
        std::array<future_t, numThreads> imageFutures{
            sequoia::utilities::make_array<future_t, numThreads>(
                [&imagePromises](std::size_t i) { return imagePromises[i].get_future(); }
            )
        };

        std::latch holdYourHorses{numThreads};

        std::array<std::jthread, numThreads> workers{
            sequoia::utilities::make_array<std::jthread, numThreads>(
                [this, &imagePromises, &holdYourHorses](std::size_t i){
                    return
                        std::jthread{
                             [this, &imagePromises, &holdYourHorses, i](std::promise<unique_image> p) {
                                 holdYourHorses.arrive_and_wait();
                                 if(i % 2)
                                     p.set_value(unique_image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::no,  all_channels_in_image});
                                 else
                                     p.set_value(unique_image{working_materials() / "bgr_striped_2w_3h_3c.png", flip_vertically::yes, all_channels_in_image});
                             },
                             std::move(imagePromises[i])
                        };
                }
            )
        };

        const bool allPassed{
            [numThreads, &imageFutures]() {
                for(auto i : std::views::iota(0uz, numThreads)) {
                    const auto comparison{i % 2 ? make_bgr_striped(2, 3, colour_channels{3}, alignment{1}) : make_rgb_striped(2, 3, colour_channels{3}, alignment{1})};
                    if(!std::ranges::equal(imageFutures[i].get().span(), comparison.data))
                        return false;
                }

                return true;
            }()
        };
 
       check("", allPassed);
    }
}
