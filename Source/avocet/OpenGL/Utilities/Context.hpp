////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <concepts>
#include <functional>

#include "glad/gl.h"

namespace avocet::opengl {
    class extended_context {
        GladGLContext m_Context{};
        std::function<void()> m_gl_function_prologue{};
        std::function<void()> m_gl_function_epilogue{};
    public:
        extended_context() = default;

        template<std::invocable<GladGLContext&> Loader, class Prologue = std::function<void()>, class Epilogue = std::function<void()>>
            requires std::is_invocable_r_v<void, Prologue> && std::is_invocable_r_v<void, Epilogue>
        explicit extended_context(Loader loader, Prologue prologue = {}, Epilogue epilogue = {})
            : m_gl_function_prologue{std::move(prologue)}
            , m_gl_function_epilogue{std::move(epilogue)}
        {
            loader(m_Context);
        }

        void invoke_prologue() {
            if(m_gl_function_prologue)
                m_gl_function_prologue();
        }

        void invoke_epilogue() {
            if(m_gl_function_epilogue)
                m_gl_function_epilogue();
        }

        [[nodiscard]]
        const GladGLContext& glad_context() const noexcept { return m_Context; }
    };
}