////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <format>

namespace avocet::opengl{
    struct opengl_version {
        std::size_t major{4}, minor{1};

        [[nodiscard]]
        friend auto operator<=>(const opengl_version&, const opengl_version&) noexcept = default;
    };

    [[nodiscard]]
    constexpr bool debug_output_supported(opengl_version version) noexcept {
        return (version.major > 3) && (version.minor >= 3);
    }
}


namespace std {
    template<>
    struct formatter<avocet::opengl::opengl_version> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::opengl::opengl_version version, auto& ctx) const {
            return format_to(ctx.out(), "{}.{}", version.major, version.minor);
        }
    };
}
