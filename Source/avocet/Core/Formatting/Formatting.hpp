////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <format>
#include <type_traits>
#include <source_location>

namespace avocet {
    template<class T>
    inline constexpr bool has_to_string{
        requires(const T & t) {
            { to_string(t) } -> std::convertible_to<std::string>;
        }
    };

    template<std::integral T>
    std::string make_error_message(std::string_view enumName, T val) {
        return std::format("Unrecognized option {}::{} while stringifying", enumName, val);
    }

    template<class Enum>
        requires std::is_scoped_enum_v<Enum>
    [[nodiscard]]
    std::string error_message(std::string_view enumName, Enum val) {
        return make_error_message(enumName, static_cast<std::underlying_type_t<Enum>>(val));
    }

    [[nodiscard]]
    std::string to_string(std::source_location loc);
}

namespace std {
    template<class Enum>
        requires std::is_scoped_enum_v<Enum> && avocet::has_to_string<Enum>
    struct formatter<Enum> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(Enum val, auto& ctx) const {
            return format_to(ctx.out(), "{}", to_string(val));
        }
    };
}