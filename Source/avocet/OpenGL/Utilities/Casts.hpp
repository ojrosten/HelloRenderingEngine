////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"

#include <type_traits>

namespace avocet::opengl {
    [[nodiscard]]
    constexpr GLsizei to_gl_sizei(std::size_t val) noexcept { return static_cast<GLsizei>(val); }

    template<class T>
        requires std::is_scoped_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, GLenum>
    [[nodiscard]]
    constexpr GLenum to_gl_enum(T val) noexcept { return static_cast<GLenum>(val); }

    template<class T>
        requires (std::is_scoped_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, GLint>) || std::is_integral_v<T>
    [[nodiscard]]
    constexpr GLint to_gl_int(T val) noexcept { return static_cast<GLint>(val); }

    template<class T>
        requires (std::is_scoped_enum_v<T>&& std::is_same_v<std::underlying_type_t<T>, GLboolean>)
    [[nodiscard]]
    constexpr GLboolean to_gl_boolean(T val) noexcept { return static_cast<GLboolean>(val); }
}