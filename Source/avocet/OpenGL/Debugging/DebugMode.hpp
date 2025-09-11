////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"

#include <type_traits>

namespace avocet::opengl {
    enum class debugging_mode { none = 0, dynamic };

    [[nodiscard]]
    constexpr debugging_mode inferred_debugging_mode() noexcept {
        if constexpr(has_ndebug())
            return debugging_mode::none;
        else
            return debugging_mode::dynamic;
    }

    template<debugging_mode Mode>
    struct debugging_mode_constant : std::integral_constant<debugging_mode, Mode> {};

    using debugging_mode_off_type     = debugging_mode_constant<debugging_mode::none>;
    using debugging_mode_dynamic_type = debugging_mode_constant<debugging_mode::dynamic>;

    inline constexpr debugging_mode_off_type debugging_mode_off{};
}