////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Core/RenderArea/Viewport.hpp"

namespace avocet {
    namespace {
        [[nodiscard]]
        constexpr bool good_extent(const discrete_extent& extent) noexcept {
            return (extent.width > 0) && (extent.height > 0);
        }
    }

    /// <summary>
    /// Given a nominal extent and an available extent
    /// - If any width or height is zero avocet::null_viewport is returned;
    /// - Otherwise an attempt is made to construct a viewport that:
    ///     1. Preserves the aspect ratio of the nominal extent
    ///     2. Is as big as possible
    ///     3. Is centralized
    /// In the event that the constraints cannot be met exactly:
    ///   - It is unspecified the precision to which each constraint is met
    /// </summary>
    [[nodiscard]]
    std::optional<viewport> refit(const discrete_extent& nominalExtent, const discrete_extent& availableExtent) {
        if(!good_extent(nominalExtent) || !good_extent(availableExtent))
            return null_viewport;

        const double   nominalAspectRatio{static_cast<double>(  nominalExtent.width) /   nominalExtent.height},
                     availableAspectRatio{static_cast<double>(availableExtent.width) / availableExtent.height};

        if(availableAspectRatio < nominalAspectRatio) {
            const std::uint32_t   width{availableExtent.width};
            const std::uint32_t  height{static_cast<std::uint32_t>(width / nominalAspectRatio)};
            const std::int32_t  delta_y{static_cast<std::int32_t>((availableExtent.height - height) / 2)};
            if(height > 0)
                return viewport{{0, delta_y}, {width, height}};
        }
        else if(availableAspectRatio > nominalAspectRatio) {
            const std::uint32_t height{availableExtent.height};
            const std::uint32_t width{static_cast<std::uint32_t>(height * nominalAspectRatio)};
            const std::int32_t  delta_x{static_cast<std::int32_t>((availableExtent.width - width) / 2)};
            if(width > 0)
                return viewport{{delta_x, 0}, {width, height}};
        }
        else {
            return viewport{{}, availableExtent};
        }

        return null_viewport;
    }
}
