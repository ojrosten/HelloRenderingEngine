////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "UniqueImageThreadingFreeTest.hpp"
#include "ImageTestingUtilities.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"
#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"

#include <future>
#include <latch>
#include <thread>

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path unique_image_threading_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void unique_image_threading_free_test::run_tests()
    {
        constexpr std::size_t numThreads{8};
        using promise_t = std::promise<unique_image>;
        using future_t = std::future<unique_image>;
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
                                 const auto flip{i % 2 ? flip_vertically::no : flip_vertically::yes};
                                 p.set_value(unique_image{working_materials() / "bgr_striped_2w_3h_3c.png", flip, all_channels_in_image});
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
