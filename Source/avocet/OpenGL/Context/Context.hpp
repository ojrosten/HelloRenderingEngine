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

    enum class object_labelling_available {
        no,
        yes,
        driver_dependent
    };

    class context_debug_characteristics {
        bool m_DebugOutputEnabled{};
        object_labelling_available m_ObjectLabelsAvailable{};
        std::optional<std::size_t> m_MaxDebugMessageLength{};
    public:
        explicit context_debug_characteristics(const context_base& ctx);

        [[nodiscard]]
        bool debug_output_enabled() const noexcept { return m_DebugOutputEnabled; }

        [[nodiscard]]
        object_labelling_available object_labels_available() const noexcept { return m_ObjectLabelsAvailable; }

        [[nodiscard]]
        std::optional<std::size_t> max_debug_message_length() const noexcept { return m_MaxDebugMessageLength; }
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
        const context_debug_characteristics& debug_characteristics() const noexcept { return m_Characteristics; }
    protected:
        ~context() = default;

        context(context&&) noexcept = default;

        context& operator=(context&&) noexcept = default;
    private:
        context_debug_characteristics m_Characteristics;
    };
}