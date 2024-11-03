////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Debugging/OpenGL/DebugMode.hpp"
#include "avocet/Utilities/OpenGL/Version.hpp"

#include <source_location>
#include <string>

namespace avocet::opengl {
    [[nodiscard]]
    std::string to_string(std::source_location loc);

    void check_for_basic_errors(std::source_location loc);

    void check_for_advanced_errors(std::source_location loc);

    inline void check_for_errors(std::source_location loc) {
        if constexpr(inferred_debugging_mode() == debugging_mode::basic)
            check_for_basic_errors(loc);
        else if constexpr(inferred_debugging_mode() == debugging_mode::advanced)
            check_for_advanced_errors(loc);
        else if constexpr(inferred_debugging_mode() == debugging_mode::dynamic) {
            if(const auto version{get_opengl_version()}; (version.major > 3) && (version.minor >= 3))
                check_for_advanced_errors(loc);
            else
                check_for_basic_errors(loc);
        }
    }
}