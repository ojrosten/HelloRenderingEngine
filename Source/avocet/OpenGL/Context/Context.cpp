////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/ContextBase.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Context/Version.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::size_t get_max_debug_message_length(const context_base& ctx) {
            if(!debug_output_supported(ctx))
                return 0;

            GLint maxLen{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_DEBUG_MESSAGE_LENGTH, &maxLen);
            if(maxLen < 0)
                throw std::runtime_error{std::format("Max Debug Length is Negative: {}", maxLen)};

            return static_cast<std::size_t>(maxLen);
        }
    }

    context_characteristics::context_characteristics(const context_base& ctx)
        : m_MaxDebugMessageLength{get_max_debug_message_length(ctx)}
    {
    }
}