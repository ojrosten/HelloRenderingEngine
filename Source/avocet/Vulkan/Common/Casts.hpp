////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <cstdint>

namespace avocet::vulkan {
    [[nodiscard]]
    constexpr std::uint32_t to_uint32(std::size_t i) { return static_cast<std::uint32_t>(i); }
}