////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/ContextBase.hpp"
#include "avocet/OpenGL/Context/GLGetters.hpp"

#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::size_t get_max_debug_message_length(const context_base& ctx, const bool debugSupported) {
            if(!debugSupported)
                return 0;

            return get<GLint>(ctx, GL_MAX_DEBUG_MESSAGE_LENGTH);
        }
    }

    context_debug_characteristics::context_debug_characteristics(const context_base& ctx)
        : m_DebugOutputSupported{opengl::debug_output_supported(ctx.version()) && (ctx.debug_mode() == debugging_mode::dynamic)}
        , m_MaxDebugMessageLength{get_max_debug_message_length(ctx, m_DebugOutputSupported)}
    {
    }
}