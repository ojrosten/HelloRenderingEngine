////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Preprocessor/Core/PreprocessorDefs.hpp"

namespace avocet::opengl {
    enum class debugging_mode { none = 0, basic, advanced };

    [[nodiscard]]
    constexpr debugging_mode requested_debugging_mode() noexcept {
        if constexpr(has_ndebug())
            return debugging_mode::none;
        else if constexpr(is_apple())
            return debugging_mode::basic;
        else
            return debugging_mode::advanced;
    }
}