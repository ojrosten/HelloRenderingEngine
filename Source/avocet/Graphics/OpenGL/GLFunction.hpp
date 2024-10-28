////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Errors.hpp"

#include <concepts>
#include <stdexcept>

namespace avocet::opengl {
    enum class debugging_mode { none = 0, basic };

    [[nodiscard]]
    constexpr debugging_mode get_debugging_mode() noexcept {
        #if defined(NDEBUG)
            return debugging_mode::none;
        #else
            return debugging_mode::basic;
        #endif
    }

    inline void check_for_errors(std::source_location loc) {
        if constexpr(get_debugging_mode() == debugging_mode::basic)
            check_for_basic_errors(loc);
    }
}