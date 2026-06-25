////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include <utility>

#include "glad/gl.h"

namespace avocet::opengl {

    /// Function to convert an OpenGL enumeration to its underlying type, such that
    /// the latter must be spelled out. This function is designed for use at the
    /// C++/OpenGL boundary, where it can be very helpful to see the OpenGL types
    /// involved stated explicitly.
    template<gl_underlies_enum UnderlyingType, class Enum>
        requires std::is_scoped_enum_v<Enum> && std::same_as<UnderlyingType, std::underlying_type_t<Enum>>
    [[nodiscard]]
    constexpr UnderlyingType to_gl_underlying_value(Enum e) noexcept {
        return std::to_underlying(e);
    }
}