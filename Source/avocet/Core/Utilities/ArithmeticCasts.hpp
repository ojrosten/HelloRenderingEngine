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

namespace avocet {
    template<std::integral From, std::integral To>
    inline constexpr bool safe_integral_conversion_v{sequoia::initializable_from<To, From>};

    template<std::integral To, std::integral From>
    [[nodiscard]]
    constexpr To checked_conversion_to(From val) noexcept(safe_integral_conversion_v<From, To>)
    {
        if constexpr (!safe_integral_conversion_v<From, To>) {
            if (constexpr auto maxTo{std::numeric_limits<To>::max()}; val > maxTo)
                throw std::domain_error{std::format("Unable to safely convert {} to an integral type with a maximum value of {}", val, maxTo)};


            if constexpr (std::is_signed_v<From>) {
                if constexpr (std::is_signed_v<To>) {
                    if (constexpr auto lowestTo{std::numeric_limits<To>::lowest()}; val < lowestTo)
                        throw std::domain_error{std::format("Unable to safely convert {} to an integral type with a minimum value of {}", val, lowestTo)};
                }
                else if (val < 0) {
                    throw std::domain_error{std::format("Unable to safely convert {} to an unsigned integral type", val)};
                }
            }
        }

        return static_cast<To>(val);
    }

    template<class T>
        requires std::is_scoped_enum_v<T>
    [[nodiscard]]
    constexpr std::underlying_type_t<T> to_underlying_value(T val) noexcept { return static_cast<std::underlying_type_t<T>>(val); }
}