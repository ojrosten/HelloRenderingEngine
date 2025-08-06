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
        using promise_t = std::promise<unique_image>;
        using future_t  = std::future<unique_image>;

        constexpr std::size_t numThreads{8};

        std::array<promise_t, numThreads> imagePromises{};

        auto imageFutures{
            sequoia::utilities::make_array<future_t, numThreads>(
                [&imagePromises](std::size_t i) {
                    return imagePromises[i].get_future();
                }
            )
        };

        {
            std::latch holdYourHorses{numThreads};
            const auto imagePath{working_materials() / "bgr_striped_2w_3h_3c.png"};

            auto workers{
                sequoia::utilities::make_array<std::jthread, numThreads>(
                    [&imagePromises, &holdYourHorses, &imagePath](std::size_t i) {
                        return std::jthread{
                            [&holdYourHorses, &imagePath](promise_t p, std::size_t i) {
                                const auto flip{i % 2 ? flip_vertically::yes : flip_vertically::no};
                                holdYourHorses.arrive_and_wait();
                                p.set_value(unique_image{imagePath, flip, all_channels_in_image});
                            },
                            std::move(imagePromises[i]),
                            i
                        };
                    }
                )
            };
        }

        const bool passed{
            [numThreads, &imageFutures](){
                for(auto i : std::views::iota(0uz, numThreads)) {
                    const auto comparison{i % 2 ? make_rgb_striped(2, 3, colour_channels{3}, alignment{1})
                                                : make_bgr_striped(2, 3, colour_channels{3}, alignment{1})};

                    if(not std::ranges::equal(imageFutures[i].get().span(), comparison.data))
                        return false;
                }

                return true;
            }()
        };

        check("All images correct", passed);
    }
}
