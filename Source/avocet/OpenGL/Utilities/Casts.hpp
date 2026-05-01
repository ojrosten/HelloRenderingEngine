////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

#include "glad/gl.h"

namespace avocet::opengl {
    template<std::unsigned_integral From>
    [[nodiscard]]
    constexpr GLsizei to_gl_sizei(From val) noexcept(safe_integral_conversion_v<From, GLsizei>) { return convert_value_to<GLsizei>(val); }

    template<std::integral From>
    [[nodiscard]]
    constexpr GLint to_gl_int(From val) noexcept(safe_integral_conversion_v<From, GLint>) { return convert_value_to<GLint>(val); }

    template<class T>
        requires std::is_scoped_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, GLenum>
    [[nodiscard]]
    constexpr GLenum to_gl_enum(T val) noexcept { return to_underlying_value(val); }

    template<class T>
        requires std::is_scoped_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, GLint>
    [[nodiscard]]
    constexpr GLint to_gl_int(T val) noexcept { return to_underlying_value(val); }

    template<class T>
        requires (std::is_scoped_enum_v<T>&& std::is_same_v<std::underlying_type_t<T>, GLboolean>)
    [[nodiscard]]
    constexpr GLboolean to_gl_boolean(T val) noexcept { return to_underlying_value(val); }
}