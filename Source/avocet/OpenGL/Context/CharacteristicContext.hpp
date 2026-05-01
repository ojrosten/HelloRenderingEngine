////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/DecoratedContext.hpp"

namespace avocet::opengl {

    class context_characteristics {
        std::string m_Vendor{},
                   m_Renderer{};

        GLint m_MaxLabelLength{};
    public:
        explicit context_characteristics(const decorated_context& ctx);

        [[nodiscard]]
        const std::string& vendor() const noexcept { return m_Vendor; }

        [[nodiscard]]
        const std::string& renderer() const noexcept { return m_Renderer; }

        [[nodiscard]]
        GLint max_label_length() const noexcept { return m_MaxLabelLength; }
    };

    class characteristic_context : public decorated_context {
    public:
        template<class Fn>
        constexpr static bool is_decorator_v{decorated_context::is_decorator_v<Fn>};

        template<class Loader, class Prologue, class Epilogue>
            requires is_decorator_v<Prologue>
                  && is_decorator_v<Epilogue>
                  && std::is_invocable_r_v<GladGLContext, Loader, GladGLContext>
        characteristic_context(debugging_mode mode, Loader loader, Prologue prologue, Epilogue epilogue)
            : decorated_context{mode, std::move(loader), std::move(prologue), std::move(epilogue)}
            , m_Characteristics{static_cast<const decorated_context&>(*this)}
        {}

        [[nodiscard]]
        const context_characteristics& characteristics() const noexcept { return m_Characteristics; }
    protected:
        ~characteristic_context() = default;

        characteristic_context(characteristic_context&&) noexcept = default;

        characteristic_context& operator=(characteristic_context&&) noexcept = default;
    private:
        context_characteristics m_Characteristics;
    };
}