////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"

#include <concepts>

namespace avocet::opengl {
    template<class T>
    concept gl_arithmetic_type = 
           std::is_same_v<T, GLhalf> || std::is_same_v<T, GLfloat> || std::is_same_v<T, GLdouble> || std::is_same_v<T, GLfixed>
        || std::is_same_v<T, GLbyte> || std::is_same_v<T, GLubyte> || std::is_same_v<T, GLshort>  || std::is_same_v<T, GLint>
        || std::is_same_v<T, GLuint>;

    template<class T>
    concept gl_floating_point = std::is_same_v<T, GLfloat> || std::is_same_v<T, GLdouble>;

    enum class gl_type_specifier : GLenum {
        gl_half   = GL_HALF_FLOAT,
        gl_float  = GL_FLOAT,
        gl_double = GL_DOUBLE,
        gl_fixed  = GL_FIXED,
        gl_byte   = GL_BYTE,
        gl_ubyte  = GL_UNSIGNED_BYTE,
        gl_short  = GL_SHORT,
        gl_ushort = GL_UNSIGNED_SHORT,
        gl_int    = GL_INT,
        gl_uint   = GL_UNSIGNED_INT,
    };

    template<gl_arithmetic_type T>
    struct to_gl_type_specifier;

    template<gl_arithmetic_type T>
    inline constexpr auto to_gl_type_specifier_v{to_gl_type_specifier<T>::value};

    template<>
    struct to_gl_type_specifier<GLfloat> {
        constexpr static auto value{gl_type_specifier::gl_float};
    };

    template<>
    struct to_gl_type_specifier<GLdouble> {
        constexpr static auto value{gl_type_specifier::gl_double};
    };

    template<>
    struct to_gl_type_specifier<GLbyte> {
        constexpr static auto value{gl_type_specifier::gl_byte};
    };

    template<>
    struct to_gl_type_specifier<GLubyte> {
        constexpr static auto value{gl_type_specifier::gl_ubyte};
    };

    template<>
    struct to_gl_type_specifier<GLshort> {
        constexpr static auto value{gl_type_specifier::gl_short};
    };

    template<>
    struct to_gl_type_specifier<GLushort> {
        constexpr static auto value{gl_type_specifier::gl_ushort};
    };

    template<>
    struct to_gl_type_specifier<GLint> {
        constexpr static auto value{gl_type_specifier::gl_int};
    };

    template<>
    struct to_gl_type_specifier<GLuint> {
        constexpr static auto value{gl_type_specifier::gl_uint};
    };
}