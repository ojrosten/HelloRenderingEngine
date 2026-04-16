////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/ContextBase.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::string get_vendor(const context_base& ctx) {
            return {std::bit_cast<const char*>(gl_function{&GladGLContext::GetString}(ctx, GL_VENDOR))};
        }

        [[nodiscard]]
        std::string get_renderer(const context_base& ctx) {
            return {std::bit_cast<const char*>(gl_function{&GladGLContext::GetString}(ctx, GL_RENDERER))};
        }

        [[nodiscard]]
        std::size_t get_max_debug_message_length(const context_base& ctx, const bool debugSupported) {
            if(!debugSupported)
                return 0;

            GLint maxLen{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_DEBUG_MESSAGE_LENGTH, &maxLen);
            if(maxLen < 0)
                throw std::runtime_error{std::format("Max Debug Length is Negative: {}", maxLen)};

            return static_cast<std::size_t>(maxLen);
        }
    }

    context_characteristics::context_characteristics(const context_base& ctx)
        : m_Vendor{get_vendor(ctx)}
        , m_Renderer{get_renderer(ctx)}
        , m_DebugOutputSupported{opengl::debug_output_supported(ctx.version()) && (ctx.debug_mode() == debugging_mode::dynamic)}
        , m_MaxDebugMessageLength{get_max_debug_message_length(ctx, m_DebugOutputSupported)}
    {
    }
}