////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"

namespace avocet::opengl {
    enum class debugging_mode { none = 0, basic, advanced, dynamic };

    [[nodiscard]]
    constexpr debugging_mode inferred_debugging_mode() noexcept {
        if constexpr(has_ndebug())
            return debugging_mode::none;
        else if constexpr(is_windows())
            return debugging_mode::advanced;
        else if constexpr(is_apple())
            return debugging_mode::basic;
        else
            return debugging_mode::dynamic;
    }
}