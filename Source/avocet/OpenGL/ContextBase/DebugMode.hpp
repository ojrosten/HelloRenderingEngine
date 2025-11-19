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
    enum class debugging_mode { off = 0, dynamic };

    template<debugging_mode Mode>
    struct debugging_mode_constant : std::integral_constant<debugging_mode, Mode> {};

    using debugging_mode_off_type = debugging_mode_constant<debugging_mode::off>;

    inline constexpr debugging_mode_off_type debugging_mode_off{};
}