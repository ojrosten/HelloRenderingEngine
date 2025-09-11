////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/DebugMode.hpp"

#include <concepts>
#include <functional>
#include <source_location>

#include "glad/gl.h"

namespace avocet::opengl {
    class extended_context;

    using prologue_function_type = std::function<void(extended_context, debugging_mode, std::source_location)>;
    using epilogue_function_type = std::function<void(extended_context, debugging_mode, std::source_location)>;

    inline const prologue_function_type null_prologue{};
    inline const epilogue_function_type null_epilogue{};

    class extended_context {
        GladGLContext m_Context{};
        prologue_function_type m_gl_function_prologue{};
        epilogue_function_type m_gl_function_epilogue{};
    public:
        extended_context() = default;

        template<std::invocable<GladGLContext&> Loader, class Prologue = std::function<void()>, class Epilogue = std::function<void()>>
            requires std::is_invocable_r_v<void, Prologue, extended_context, debugging_mode, std::source_location> && std::is_invocable_r_v<void, Epilogue, extended_context, debugging_mode, std::source_location>
        explicit extended_context(Loader loader, Prologue prologue = {}, Epilogue epilogue = {})
            : m_gl_function_prologue{std::move(prologue)}
            , m_gl_function_epilogue{std::move(epilogue)}
        {
            loader(m_Context);
        }

        void invoke_prologue(debugging_mode mode, std::source_location loc) const {
            if(m_gl_function_prologue)
                m_gl_function_prologue(*this, mode, loc);
        }

        void invoke_epilogue(debugging_mode mode, std::source_location loc) const {
            if(m_gl_function_epilogue)
                m_gl_function_epilogue(*this, mode, loc);
        }

        [[nodiscard]]
        const GladGLContext& glad_context() const noexcept { return m_Context; }
    };
}