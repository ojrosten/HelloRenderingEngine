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
        gl_float  = GL_FLOAT,
        gl_double = GL_DOUBLE
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
}