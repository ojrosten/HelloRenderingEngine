////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/Context.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::optional<std::size_t> get_max_debug_message_length(const context_base& ctx, const bool debugEnabled) {
            if(!debugEnabled)
                return std::nullopt;

            GLint maxLen{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_DEBUG_MESSAGE_LENGTH, &maxLen);
            if(maxLen < 0)
                throw std::runtime_error{std::format("Max Debug Length is Negative: {}", maxLen)};

            return static_cast<std::size_t>(maxLen);
        }

        [[nodiscard]]
        object_labelling_available labelling_available(const opengl_version& version, const bool debugEnabled) {
            return   !object_labels_supported(version) ? object_labelling_available::no
                   : debugEnabled                      ? object_labelling_available::yes
                                                       : object_labelling_available::driver_dependent;
        }
    }

    context_debug_characteristics::context_debug_characteristics(const context_base& ctx)
        : m_DebugOutputEnabled{opengl::debug_output_supported(ctx.version()) && (ctx.debug_mode() == debugging_mode::dynamic)}
        , m_ObjectLabelsAvailable{labelling_available(ctx.version(), m_DebugOutputEnabled)}
        , m_MaxDebugMessageLength{get_max_debug_message_length(ctx, m_DebugOutputEnabled)}
    {
    }
}