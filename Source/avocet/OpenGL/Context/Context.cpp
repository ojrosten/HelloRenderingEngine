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
        std::string get_vendor(const context_base& ctx) {
            return {std::bit_cast<const char*>(gl_function{&GladGLContext::GetString}(ctx, GL_VENDOR))};
        }

        [[nodiscard]]
        std::string get_renderer(const context_base& ctx) {
            return {std::bit_cast<const char*>(gl_function{&GladGLContext::GetString}(ctx, GL_RENDERER))};
        }

        [[nodiscard]]
        std::optional<std::size_t> get_max_debug_message_length(const context_base& ctx, const bool debugSupported) {
            if(!debugSupported)
                return std::nullopt;

            GLint maxLen{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_DEBUG_MESSAGE_LENGTH, &maxLen);
            if(maxLen < 0)
                throw std::runtime_error{std::format("Max Debug Length is Negative: {}", maxLen)};

            return static_cast<std::size_t>(maxLen);
        }

        [[nodiscard]]
        object_labelling_available labelling_available(const opengl_version& version, const bool debugEnabled) {
            if(!object_labels_supported(version))
                return object_labelling_available::no;

            if(debugEnabled)
                return object_labelling_available::yes;

            return object_labelling_available::driver_dependent;
        }
    }

    context_characteristics::context_characteristics(const context_base& ctx)
        : m_Vendor{get_vendor(ctx)}
        , m_Renderer{get_renderer(ctx)}
        , m_DebugOutputEnabled{opengl::debug_output_supported(ctx.version()) && (ctx.debug_mode() == debugging_mode::dynamic)}
        , m_ObjectLabelsAvailable{labelling_available(ctx.version(), m_DebugOutputEnabled)}
        , m_MaxDebugMessageLength{get_max_debug_message_length(ctx, m_DebugOutputEnabled)}
    {
    }
}