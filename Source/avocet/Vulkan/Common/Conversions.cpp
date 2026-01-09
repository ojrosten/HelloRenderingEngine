////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Common/Conversions.hpp"

#include <ranges>

namespace avocet::vulkan {
    [[nodiscard]]
    std::vector<const char*> to_cstyle_strings(std::span<const std::string> strs) {
        return
            std::views::transform(
                strs,
                [](const std::string& s) -> const char* { return s.data(); }
            ) | std::ranges::to<std::vector>();
    }
}