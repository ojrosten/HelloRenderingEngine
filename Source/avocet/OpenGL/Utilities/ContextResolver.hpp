////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"
#include <stdexcept>

namespace avocet::opengl {
    // Forward declaration - implementation provided by curlew layer
    GladGLContext* get_current_context();
    
    // Context-aware function resolver
    // This allows us to get context-specific function pointers for use with gl_function
    template<typename T>
    T get_context_function(T GladGLContext::* member_ptr) {
        auto* context = get_current_context();
        if (!context) {
            throw std::runtime_error("No current OpenGL context available");
        }
        return context->*member_ptr;
    }

    // Safe context function resolver that returns nullptr instead of throwing
    template<typename T>
    T get_context_function_safe(T GladGLContext::* member_ptr) {
        auto* context = get_current_context();
        if (!context) {
            return nullptr;
        }
        return context->*member_ptr;
    }
}

// Global function pointer accessors that resolve to current context
// This maintains compatibility with existing gl_function{glXXX} usage
#define glGetString        avocet::opengl::get_context_function(&GladGLContext::GetString)
#define glGetIntegerv      avocet::opengl::get_context_function(&GladGLContext::GetIntegerv)
#define glGetError         avocet::opengl::get_context_function(&GladGLContext::GetError)
#define glEnable           avocet::opengl::get_context_function(&GladGLContext::Enable)
#define glDebugMessageControl avocet::opengl::get_context_function(&GladGLContext::DebugMessageControl)
#define glObjectLabel      avocet::opengl::get_context_function(&GladGLContext::ObjectLabel)
#define glGetObjectLabel   avocet::opengl::get_context_function(&GladGLContext::GetObjectLabel)
#define glCreateProgram    avocet::opengl::get_context_function(&GladGLContext::CreateProgram)
#define glDeleteProgram    avocet::opengl::get_context_function(&GladGLContext::DeleteProgram)
#define glCreateShader     avocet::opengl::get_context_function(&GladGLContext::CreateShader)
#define glDeleteShader     avocet::opengl::get_context_function(&GladGLContext::DeleteShader)
#define glShaderSource     avocet::opengl::get_context_function(&GladGLContext::ShaderSource)
#define glCompileShader    avocet::opengl::get_context_function(&GladGLContext::CompileShader)
#define glAttachShader     avocet::opengl::get_context_function(&GladGLContext::AttachShader)
#define glDetachShader     avocet::opengl::get_context_function(&GladGLContext::DetachShader)
#define glLinkProgram      avocet::opengl::get_context_function(&GladGLContext::LinkProgram)
#define glUseProgram       avocet::opengl::get_context_function(&GladGLContext::UseProgram)
#define glGetShaderiv      avocet::opengl::get_context_function(&GladGLContext::GetShaderiv)
#define glGetShaderInfoLog avocet::opengl::get_context_function(&GladGLContext::GetShaderInfoLog)
#define glGetProgramiv     avocet::opengl::get_context_function(&GladGLContext::GetProgramiv)
#define glGetProgramInfoLog avocet::opengl::get_context_function(&GladGLContext::GetProgramInfoLog)
#define glGetUniformLocation avocet::opengl::get_context_function(&GladGLContext::GetUniformLocation)
#define glUniform1f        avocet::opengl::get_context_function(&GladGLContext::Uniform1f)
#define glUniform1i        avocet::opengl::get_context_function(&GladGLContext::Uniform1i)
#define glUniform2f        avocet::opengl::get_context_function(&GladGLContext::Uniform2f)
#define glGenTextures      avocet::opengl::get_context_function(&GladGLContext::GenTextures)
#define glDeleteTextures   avocet::opengl::get_context_function(&GladGLContext::DeleteTextures)
#define glBindTexture      avocet::opengl::get_context_function(&GladGLContext::BindTexture)
#define glActiveTexture    avocet::opengl::get_context_function(&GladGLContext::ActiveTexture)
#define glPixelStorei      avocet::opengl::get_context_function(&GladGLContext::PixelStorei)
#define glTexImage2D       avocet::opengl::get_context_function(&GladGLContext::TexImage2D)
#define glGetTexImage      avocet::opengl::get_context_function(&GladGLContext::GetTexImage)
#define glGetTexLevelParameteriv avocet::opengl::get_context_function(&GladGLContext::GetTexLevelParameteriv)
#define glGetDebugMessageLog avocet::opengl::get_context_function(&GladGLContext::GetDebugMessageLog)
#define glGenVertexArrays  avocet::opengl::get_context_function(&GladGLContext::GenVertexArrays)
#define glDeleteVertexArrays avocet::opengl::get_context_function(&GladGLContext::DeleteVertexArrays)
#define glBindVertexArray  avocet::opengl::get_context_function(&GladGLContext::BindVertexArray)
#define glGenBuffers       avocet::opengl::get_context_function(&GladGLContext::GenBuffers)
#define glDeleteBuffers    avocet::opengl::get_context_function(&GladGLContext::DeleteBuffers)
#define glBindBuffer       avocet::opengl::get_context_function(&GladGLContext::BindBuffer)
#define glBufferData       avocet::opengl::get_context_function(&GladGLContext::BufferData)
#define glVertexAttribLPointer avocet::opengl::get_context_function(&GladGLContext::VertexAttribLPointer)
#define glVertexAttribPointer avocet::opengl::get_context_function(&GladGLContext::VertexAttribPointer)
#define glEnableVertexAttribArray avocet::opengl::get_context_function(&GladGLContext::EnableVertexAttribArray)
#define glGetBufferSubData avocet::opengl::get_context_function(&GladGLContext::GetBufferSubData)
#define glGetBufferParameteriv avocet::opengl::get_context_function(&GladGLContext::GetBufferParameteriv)
#define glDrawElements     avocet::opengl::get_context_function(&GladGLContext::DrawElements)
#define glDrawArrays       avocet::opengl::get_context_function(&GladGLContext::DrawArrays)
#define glTexParameteri    avocet::opengl::get_context_function(&GladGLContext::TexParameteri)
#define glClearColor       avocet::opengl::get_context_function(&GladGLContext::ClearColor)
#define glClear            avocet::opengl::get_context_function(&GladGLContext::Clear)
