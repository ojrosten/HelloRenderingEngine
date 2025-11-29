////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/ContextBase.hpp"

namespace avocet::opengl {
    struct decorator_data {
        std::string_view fn_name;
        std::source_location loc;
    };

    class context_characteristics {
        std::size_t m_MaxDebugMessageLength{};
    public:
        explicit context_characteristics(const context_base& ctx);

        [[nodiscard]]
        std::size_t max_debug_message_length() const noexcept { return m_MaxDebugMessageLength; }
    };

    class context : public context_base {
    public:
        template<class Loader>
            requires std::is_invocable_r_v<GladGLContext, Loader, GladGLContext>
        context(debugging_mode mode, Loader loader)
            : context_base{mode, std::move(loader)}
            , m_Characteristics{static_cast<const context_base&>(*this)}
        {}

        [[nodiscard]]
        const context_characteristics& characteristics() const noexcept { return m_Characteristics; }
    protected:
        ~context() = default;

        context(context&&) noexcept = default;

        context& operator=(context&&) noexcept = default;
    private:
        context_characteristics m_Characteristics;
    };
}