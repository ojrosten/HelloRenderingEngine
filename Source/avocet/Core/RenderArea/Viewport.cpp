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

    [[nodiscard]]
    std::optional<viewport> refit(const discrete_extent& nominalExtent, const discrete_extent& availableExtent) {
        return null_viewport;
    }
}
