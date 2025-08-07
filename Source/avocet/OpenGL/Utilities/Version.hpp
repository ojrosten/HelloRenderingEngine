////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"

#include <string>

#include "glad/gl.h"

namespace avocet::opengl{
    struct opengl_version {
        std::size_t major{4}, minor{1};

        [[nodiscard]]
        friend auto operator<=>(const opengl_version&, const opengl_version&) noexcept = default;
    };

    [[nodiscard]]
    std::string get_vendor(const GladGLContext& ctx);

    [[nodiscard]]
    std::string get_renderer(const GladGLContext& ctx);

    [[nodiscard]]
    std::string get_opengl_version_string(const GladGLContext& ctx);

    [[nodiscard]]
    opengl_version extract_opengl_version(const GladGLContext& ctx);
}
