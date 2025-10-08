////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/Errors.hpp"
#include "glad/gl.h"

// GLAD MX Compatibility Layer
//
// GLAD was compiled with --mx (multi-context) flag, which means there are no
// global function pointers. All OpenGL functions must be accessed through a
// GladGLContext instance.
//
// This header provides compatibility macros that resolve to the current thread's
// context, allowing existing code to continue using the familiar syntax:
//    gl_function{glGenTextures}(...)
//
// These macros resolve to: current_gl_context->GenTextures

namespace avocet::opengl {
    // Helper to get current context or throw
    inline GladGLContext& get_current_context() {
        if(!current_gl_context)
            throw std::runtime_error{"No current GL context set"};
        return *current_gl_context;
    }
}

// Define all OpenGL function pointers as macros that access the current context
// This allows backward compatibility with code written for single-context GLAD

#define glActiveTexture (avocet::opengl::get_current_context().ActiveTexture)
#define glAttachShader (avocet::opengl::get_current_context().AttachShader)
#define glBindBuffer (avocet::opengl::get_current_context().BindBuffer)
#define glBindFramebuffer (avocet::opengl::get_current_context().BindFramebuffer)
#define glBindTexture (avocet::opengl::get_current_context().BindTexture)
#define glBindVertexArray (avocet::opengl::get_current_context().BindVertexArray)
#define glBufferData (avocet::opengl::get_current_context().BufferData)
#define glBufferSubData (avocet::opengl::get_current_context().BufferSubData)
#define glClear (avocet::opengl::get_current_context().Clear)
#define glClearColor (avocet::opengl::get_current_context().ClearColor)
#define glCompileShader (avocet::opengl::get_current_context().CompileShader)
#define glCreateProgram (avocet::opengl::get_current_context().CreateProgram)
#define glCreateShader (avocet::opengl::get_current_context().CreateShader)
#define glDebugMessageControl (avocet::opengl::get_current_context().DebugMessageControl)
#define glDeleteBuffers (avocet::opengl::get_current_context().DeleteBuffers)
#define glDeleteFramebuffers (avocet::opengl::get_current_context().DeleteFramebuffers)
#define glDeleteProgram (avocet::opengl::get_current_context().DeleteProgram)
#define glDeleteShader (avocet::opengl::get_current_context().DeleteShader)
#define glDeleteTextures (avocet::opengl::get_current_context().DeleteTextures)
#define glDeleteVertexArrays (avocet::opengl::get_current_context().DeleteVertexArrays)
#define glDetachShader (avocet::opengl::get_current_context().DetachShader)
#define glDrawArrays (avocet::opengl::get_current_context().DrawArrays)
#define glDrawElements (avocet::opengl::get_current_context().DrawElements)
#define glEnable (avocet::opengl::get_current_context().Enable)
#define glEnableVertexAttribArray (avocet::opengl::get_current_context().EnableVertexAttribArray)
#define glGenBuffers (avocet::opengl::get_current_context().GenBuffers)
#define glGenFramebuffers (avocet::opengl::get_current_context().GenFramebuffers)
#define glGenTextures (avocet::opengl::get_current_context().GenTextures)
#define glGenVertexArrays (avocet::opengl::get_current_context().GenVertexArrays)
#define glGetDebugMessageLog (avocet::opengl::get_current_context().GetDebugMessageLog)
#define glGetError (avocet::opengl::get_current_context().GetError)
#define glGetIntegerv (avocet::opengl::get_current_context().GetIntegerv)
#define glGetObjectLabel (avocet::opengl::get_current_context().GetObjectLabel)
#define glGetProgramInfoLog (avocet::opengl::get_current_context().GetProgramInfoLog)
#define glGetProgramiv (avocet::opengl::get_current_context().GetProgramiv)
#define glGetShaderInfoLog (avocet::opengl::get_current_context().GetShaderInfoLog)
#define glGetShaderiv (avocet::opengl::get_current_context().GetShaderiv)
#define glGetString (avocet::opengl::get_current_context().GetString)
#define glGetUniformLocation (avocet::opengl::get_current_context().GetUniformLocation)
#define glLinkProgram (avocet::opengl::get_current_context().LinkProgram)
#define glObjectLabel (avocet::opengl::get_current_context().ObjectLabel)
#define glPixelStorei (avocet::opengl::get_current_context().PixelStorei)
#define glShaderSource (avocet::opengl::get_current_context().ShaderSource)
#define glTexImage2D (avocet::opengl::get_current_context().TexImage2D)
#define glTexParameteri (avocet::opengl::get_current_context().TexParameteri)
#define glUniform1f (avocet::opengl::get_current_context().Uniform1f)
#define glUniform1i (avocet::opengl::get_current_context().Uniform1i)
#define glUniform2f (avocet::opengl::get_current_context().Uniform2f)
#define glUniform3f (avocet::opengl::get_current_context().Uniform3f)
#define glUniform4f (avocet::opengl::get_current_context().Uniform4f)
#define glUniformMatrix4fv (avocet::opengl::get_current_context().UniformMatrix4fv)
#define glUseProgram (avocet::opengl::get_current_context().UseProgram)
#define glVertexAttribPointer (avocet::opengl::get_current_context().VertexAttribPointer)
#define glVertexAttribLPointer (avocet::opengl::get_current_context().VertexAttribLPointer)
#define glViewport (avocet::opengl::get_current_context().Viewport)
#define glGetBufferSubData (avocet::opengl::get_current_context().GetBufferSubData)
#define glGetBufferParameteriv (avocet::opengl::get_current_context().GetBufferParameteriv)
#define glGetTexImage (avocet::opengl::get_current_context().GetTexImage)
#define glGetTexLevelParameteriv (avocet::opengl::get_current_context().GetTexLevelParameteriv)
#define glReadPixels (avocet::opengl::get_current_context().ReadPixels)
#define glFramebufferTexture2D (avocet::opengl::get_current_context().FramebufferTexture2D)
#define glCheckFramebufferStatus (avocet::opengl::get_current_context().CheckFramebufferStatus)
#define glGenerateMipmap (avocet::opengl::get_current_context().GenerateMipmap)

// Version check macros
#define GLAD_GL_VERSION_1_0 (avocet::opengl::get_current_context().VERSION_1_0)
#define GLAD_GL_VERSION_1_1 (avocet::opengl::get_current_context().VERSION_1_1)
#define GLAD_GL_VERSION_1_2 (avocet::opengl::get_current_context().VERSION_1_2)
#define GLAD_GL_VERSION_1_3 (avocet::opengl::get_current_context().VERSION_1_3)
#define GLAD_GL_VERSION_1_4 (avocet::opengl::get_current_context().VERSION_1_4)
#define GLAD_GL_VERSION_1_5 (avocet::opengl::get_current_context().VERSION_1_5)
#define GLAD_GL_VERSION_2_0 (avocet::opengl::get_current_context().VERSION_2_0)
#define GLAD_GL_VERSION_2_1 (avocet::opengl::get_current_context().VERSION_2_1)
#define GLAD_GL_VERSION_3_0 (avocet::opengl::get_current_context().VERSION_3_0)
#define GLAD_GL_VERSION_3_1 (avocet::opengl::get_current_context().VERSION_3_1)
#define GLAD_GL_VERSION_3_2 (avocet::opengl::get_current_context().VERSION_3_2)
#define GLAD_GL_VERSION_3_3 (avocet::opengl::get_current_context().VERSION_3_3)
#define GLAD_GL_VERSION_4_0 (avocet::opengl::get_current_context().VERSION_4_0)
#define GLAD_GL_VERSION_4_1 (avocet::opengl::get_current_context().VERSION_4_1)
#define GLAD_GL_VERSION_4_2 (avocet::opengl::get_current_context().VERSION_4_2)
#define GLAD_GL_VERSION_4_3 (avocet::opengl::get_current_context().VERSION_4_3)
#define GLAD_GL_VERSION_4_4 (avocet::opengl::get_current_context().VERSION_4_4)
#define GLAD_GL_VERSION_4_5 (avocet::opengl::get_current_context().VERSION_4_5)
#define GLAD_GL_VERSION_4_6 (avocet::opengl::get_current_context().VERSION_4_6)
