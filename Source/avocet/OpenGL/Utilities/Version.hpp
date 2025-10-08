////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"

#include <format>

#include "glad/gl.h"

namespace avocet::opengl{
    struct opengl_version {
        std::size_t major{4}, minor{1};

        [[nodiscard]]
        friend auto operator<=>(const opengl_version&, const opengl_version&) noexcept = default;
    };

    [[nodiscard]]
    std::string get_vendor();

    [[nodiscard]]
    std::string get_vendor(GladGLContext& ctx);

    [[nodiscard]]
    std::string get_renderer();

    [[nodiscard]]
    std::string get_renderer(GladGLContext& ctx);

    [[nodiscard]]
    opengl_version get_opengl_version();

    [[nodiscard]]
    opengl_version get_opengl_version(GladGLContext& ctx);
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
