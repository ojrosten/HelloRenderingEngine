////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/Context.hpp"
#include "avocet/OpenGL/Context/GLGetters.hpp"

#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::optional<std::size_t> get_max_debug_message_length(const context_base& ctx, const bool debugEnabled) {
            if(!debugEnabled)
                return std::nullopt;

            return checked_conversion_to<std::size_t>(get(ctx, int_names::max_debug_message_length));
        }
    }

    context_debug_characteristics::context_debug_characteristics(const context_base& ctx)
        : m_MaxDebugMessageLength{get_max_debug_message_length(ctx, ctx.fundamental_characteristics().debug_output_enabled())}
    {
    }
}