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
        bool good_extent(const discrete_extent& extent) {
            return (extent.width > 0) && (extent.height > 0);
        }
    }

    /// <summary>
    /// Given a nominal extent and an available extent
    /// -   If any width or height is zero std::nullopt is returned;
    /// - Else a viewport is returned such that its extent
    ///     1. Preserves the aspect ratio of the nominal extent
    ///     2. Is as big as possible, subject to the first condition
    ///     3. Is centralized to within a pixel
    /// 
    /// In the event that the viewport's extent cannot be exactly centralized it is: 
    ///   -Unspecified which rounding mode is used
    /// </summary>
    [[nodiscard]]
    std::optional<viewport> refit(const discrete_extent& nominalExtent, const discrete_extent& availableExtent) {
        return null_viewport;
    }
}
