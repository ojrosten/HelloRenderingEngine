////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/ContextBase/ContextBase.hpp"
#include "avocet/OpenGL/ContextBase/GLFunction.hpp"
#include "avocet/OpenGL/ContextBase/Version.hpp"

namespace avocet::opengl {
    namespace fs = std::filesystem;

    [[nodiscard]]
    std::string to_string(std::source_location loc) { return std::format("{}, line {}", fs::path{loc.file_name()}.generic_string(), loc.line()); }

    void decorated_context_base::init_debug()
    {
        if(debug_mode() == debugging_mode::off)
            return;

        GLint flags{};
        gl_function{&GladGLContext::GetIntegerv}(*this, debugging_mode_off, GL_CONTEXT_FLAGS, &flags);
        if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            if(const auto version{get_opengl_version(*this)}; !debug_output_supported(version))
                throw std::runtime_error{std::format("init_debug: inconsistency between context flags {} and OpengGL version {}", flags, version)};

            gl_function{&GladGLContext::Enable}(*this, debugging_mode_off, GL_DEBUG_OUTPUT);
            gl_function{&GladGLContext::Enable}(*this, debugging_mode_off, GL_DEBUG_OUTPUT_SYNCHRONOUS);
            gl_function{&GladGLContext::DebugMessageControl}(
                *this,
                debugging_mode_off,
                GL_DONT_CARE,
                GL_DONT_CARE,
                GL_DONT_CARE,
                0,
                nullptr,
                GL_TRUE);
        }
    }
}