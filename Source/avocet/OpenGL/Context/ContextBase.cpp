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
        case dynamic: return "dynamic";
        }

        throw std::runtime_error{error_message("debugging_mode", mode)};
    }

    void context_base::init_debug() {
        if((debug_mode() == debugging_mode::off) or !debug_output_supported(*this))
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
            throw std::runtime_error{std::format("init_debug: inconsistency between context flags {} and debug mode {} / OpengGL version {}", flags, debug_mode(), get_opengl_version(*this))};
        }
    }
}
