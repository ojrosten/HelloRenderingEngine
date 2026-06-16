////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Core/Geometry/Viewport.hpp"

namespace avocet {
    namespace {
        [[nodiscard]]
        constexpr bool good_extent(const discrete_extent& extent) noexcept {
            return (extent.width > 0) && (extent.height > 0);
        }
    }

    [[nodiscard]]
    std::optional<viewport> refit(const discrete_extent& nominalExtent, const discrete_extent& availableExtent) {
        if(!good_extent(nominalExtent) || !good_extent(availableExtent))
            return null_viewport;

        const auto nominalAvailableCrossArea{std::uint64_t{  nominalExtent.width} * availableExtent.height},
                   availableNominalCrossArea{std::uint64_t{availableExtent.width} *   nominalExtent.height};

        if(availableNominalCrossArea < nominalAvailableCrossArea) {
            const std::uint32_t  width{availableExtent.width};
            const std::uint32_t height{(width * nominalExtent.height) / nominalExtent.width};
            const std::int32_t delta_y{static_cast<std::int32_t>((availableExtent.height - height) / 2)};

            if(height > 0)
                return viewport{{0, delta_y}, {width, height}};
        }
        else if(availableNominalCrossArea > nominalAvailableCrossArea) {
            const std::uint32_t height{availableExtent.height};
            const std::uint32_t  width{(height * nominalExtent.width) / nominalExtent.height};
            const std::int32_t delta_x{static_cast<std::int32_t>((availableExtent.width - width) / 2)};

            if(width > 0)
                return viewport{{delta_x, 0}, {width, height}};
        }
        else {
            return viewport{{}, availableExtent};
        }

        return null_viewport;
    }
}
