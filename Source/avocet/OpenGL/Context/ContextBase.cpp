////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/ContextBase.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Context/Version.hpp"
#include "avocet/Core/Formatting/Formatting.hpp"

namespace avocet::opengl {
    namespace fs = std::filesystem;

    [[nodiscard]]
    std::string to_string(debugging_mode mode) {
        using enum debugging_mode;
        switch(mode) {
        case off:     return "off";
        case basic:   return "basic";
        case dynamic: return "dynamic";
        }

        throw std::runtime_error{error_message("debugging_mode", mode)};
    }

    [[nodiscard]]
    opengl_version context_base::get_opengl_version() {
        if(glad_context().VERSION_4_6) return {4, 6};
        if(glad_context().VERSION_4_5) return {4, 5};
        if(glad_context().VERSION_4_4) return {4, 4};
        if(glad_context().VERSION_4_3) return {4, 3};
        if(glad_context().VERSION_4_2) return {4, 2};
        if(glad_context().VERSION_4_1) return {4, 1};
        if(glad_context().VERSION_4_0) return {4, 0};
        if(glad_context().VERSION_3_3) return {3, 3};
        if(glad_context().VERSION_3_2) return {3, 2};
        if(glad_context().VERSION_3_1) return {3, 1};
        if(glad_context().VERSION_3_0) return {3, 0};
        if(glad_context().VERSION_2_1) return {2, 1};
        if(glad_context().VERSION_2_0) return {2, 0};
        if(glad_context().VERSION_1_5) return {1, 5};
        if(glad_context().VERSION_1_4) return {1, 4};
        if(glad_context().VERSION_1_3) return {1, 3};
        if(glad_context().VERSION_1_2) return {1, 2};
        if(glad_context().VERSION_1_1) return {1, 1};

        return {1, 0};
    }

    void context_base::init_debug() {
        if((debug_mode() != debugging_mode::dynamic) or !debug_output_supported(version()))
            return;

        GLint flags{};
        gl_function{&GladGLContext::GetIntegerv}(*this, GL_CONTEXT_FLAGS, &flags);
        if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            gl_function{&GladGLContext::Enable}(*this, GL_DEBUG_OUTPUT);
            gl_function{&GladGLContext::Enable}(*this, GL_DEBUG_OUTPUT_SYNCHRONOUS);
            gl_function{&GladGLContext::DebugMessageControl}(
                *this,
                GL_DONT_CARE,
                GL_DONT_CARE,
                GL_DONT_CARE,
                0,
                nullptr,
                GL_TRUE);
        }
        else {
            throw std::runtime_error{std::format("init_debug: inconsistency between context flags {} and debug mode {} / OpengGL version {}", flags, debug_mode(), version())};
        }
    }
}
