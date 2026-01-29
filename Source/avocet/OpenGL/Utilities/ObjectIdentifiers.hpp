////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"

namespace avocet::opengl {
    enum class object_identifier : GLenum {
        buffer             = GL_BUFFER,
        shader             = GL_SHADER,
        program            = GL_PROGRAM,
        vertex_array       = GL_VERTEX_ARRAY,
        query              = GL_QUERY,
        program_pipeline   = GL_PROGRAM_PIPELINE,
        transform_feedback = GL_TRANSFORM_FEEDBACK,
        sampler            = GL_SAMPLER,
        texture            = GL_TEXTURE,
        render_buffer      = GL_RENDERBUFFER,
        framebuffer        = GL_FRAMEBUFFER
    };

    template<object_identifier Identifier>
    struct object_identifier_constant : std::integral_constant<object_identifier, Identifier>
    {};
}