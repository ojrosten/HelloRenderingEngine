////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Context/CharacteristicContext.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        std::string get_vendor(const decorated_context& ctx) {
            return {std::bit_cast<const char*>(gl_function{&GladGLContext::GetString}(ctx, GL_VENDOR))};
        }

        [[nodiscard]]
        std::string get_renderer(const decorated_context& ctx) {
            return {std::bit_cast<const char*>(gl_function{&GladGLContext::GetString}(ctx, GL_RENDERER))};
        }

        [[nodiscard]]
        GLint get_max_label_length(const decorated_context& ctx) {
            GLint param{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_LABEL_LENGTH, &param);
            return param;
        }
    }

    context_characteristics::context_characteristics(const decorated_context& ctx)
        : m_Vendor{get_vendor(ctx)}
        , m_Renderer{get_renderer(ctx)}
        , m_MaxLabelLength{get_max_label_length(ctx)}
    {
    }
}