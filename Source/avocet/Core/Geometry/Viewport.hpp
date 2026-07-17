////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Geometry/Extent.hpp"

#include <optional>

namespace avocet {
    struct viewport {
        discrete_offset offset;
        discrete_extent extent;

        [[nodiscard]]
        friend constexpr bool operator==(const viewport&, const viewport&) noexcept = default;
    };

    inline constexpr std::optional<viewport> null_viewport{};

    /// <summary>
    /// Given a nominal extent and an available extent
    /// - If any width or height is zero null_viewport is returned;
    /// - Otherwise an attempt is made to construct a viewport that:
    ///     1. Preserves the aspect ratio of the nominal extent
    ///     2. Is as big as possible
    ///     3. Is centralized
    /// In the event that the constraints cannot be met exactly:
    ///   - It is unspecified the precision to which each constraint is met
    ///   - If either the computed width or height are zero, null_viewport is returned
    /// </summary>
    [[nodiscard]]
    std::optional<viewport> refit(const discrete_extent& nominalExtent, const discrete_extent& availableExtent);

}