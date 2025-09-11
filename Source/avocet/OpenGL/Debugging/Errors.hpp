////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/DebugMode.hpp"
#include "avocet/OpenGL/Utilities/Version.hpp"

#include <source_location>
#include <string>

namespace avocet::opengl {
    [[nodiscard]]
    std::string to_string(std::source_location loc);

    struct num_messages { std::size_t value{}; };

    void check_for_basic_errors(const GladGLContext& ctx, num_messages maxNum, std::source_location loc);

    void check_for_advanced_errors(const GladGLContext& ctx, num_messages maxNum, std::source_location loc);

    [[nodiscard]]
    constexpr bool debug_output_supported(opengl_version version) noexcept {
        return (version.major > 3) && (version.minor >= 3);
    }

    [[nodiscard]]
    inline bool debug_output_supported(const GladGLContext& ctx) {
        return debug_output_supported(get_opengl_version(ctx));
    }

    [[nodiscard]]
    inline bool object_labels_activated(const GladGLContext& ctx) { return debug_output_supported(ctx); }
}
