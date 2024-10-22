////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"

#include <source_location>
#include <string>

namespace avocet::opengl {
    enum class error_codes : GLuint {
        none                          = GL_NO_ERROR,
        invalid_enum                  = GL_INVALID_ENUM,
        invalid_value                 = GL_INVALID_VALUE,
        invalid_operation             = GL_INVALID_OPERATION,
        invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
        stack_overflow                = GL_STACK_OVERFLOW,
        stack_underflow               = GL_STACK_UNDERFLOW,
        out_of_memory                 = GL_OUT_OF_MEMORY,
    };

    [[nodiscard]]
    std::string to_string(error_codes e);

    void check_for_errors(std::source_location loc);
}