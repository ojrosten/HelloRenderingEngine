////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "sequoia/Core/Meta/Concepts.hpp"

#include <format>
#include <limits>
#include <stdexcept>
#include <utility>

namespace avocet {
    template<std::integral To, std::integral From>
    inline constexpr bool has_lossless_conversion_v{
        sequoia::initializable_from<To, From>
    };

    /// If val is in the range of the return type perform a
    /// static_cast; else throw a std::domain_error
    template<std::integral To, std::integral From>
    [[nodiscard]]
    constexpr To checked_conversion_to(From val) noexcept(has_lossless_conversion_v<To, From>)
    {
        if constexpr(not has_lossless_conversion_v<To, From>) {
            if(not std::in_range<To>(val)) {
                constexpr auto lowestVal{std::numeric_limits<To>::lowest()};
                constexpr auto maxVal{std::numeric_limits<To>::max()};
                throw std::domain_error{std::format("Value {} is outside the range [{}, {}] of the target type", val, lowestVal, maxVal)};
            }
        }

        return static_cast<To>(val);
    }
}