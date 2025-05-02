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

    void check_for_basic_errors(num_messages maxNum, std::source_location loc);

    void check_for_advanced_errors(num_messages maxNum, std::source_location loc);

    [[nodiscard]]
    inline bool debug_output_supported(opengl_version version) {
        return (version.major > 3) && (version.minor >= 3);
    }

    /// <summary>
    /// This function queries and inspects the OpenGL version to determine if
    /// advanced debug output is supported. It only ever performs this query once,
    /// the first time the function is called, to avoid unnecessary overhead.
    /// Therefore, if a context with an OpenGL version < 4.3 is created, say
    /// for the purposes of determining the maximum version actually supported,
    /// it is inavisable to call this function within this context.
    /// </summary>
    [[nodiscard]]
    inline bool debug_output_supported() {
        const static bool debugOutputSupported{debug_output_supported(extract_opengl_version())};
        return debugOutputSupported;
    }

    [[nodiscard]]
    inline bool object_labels_activated() { return debug_output_supported(); }
}
