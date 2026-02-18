////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/DecoratedContext.hpp"
#include "avocet/OpenGL/ResourceInfrastructure/ResourceHandle.hpp"

namespace avocet::opengl {
    template<std::derived_from<context_base> Context>
    class generic_contextual_resource_view {
        const Context* m_Context{};
        const resource_handle* m_Handle{};
    public:
        generic_contextual_resource_view(const Context& ctx, const resource_handle& crv)
            : m_Context{&ctx}
            , m_Handle{&crv}
        {
        }

        [[nodiscard]]
        const Context& context() const noexcept { return *m_Context; }

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return *m_Handle; }

        template<class OtherContext>
            requires std::derived_from<Context, OtherContext>
        [[nodiscard]]
        operator generic_contextual_resource_view<OtherContext>() const noexcept { return {context(), handle()}; }


        [[nodiscard]]
        friend bool operator==(const generic_contextual_resource_view&, const generic_contextual_resource_view&) noexcept = default;
    };

    using contextual_resource_view = generic_contextual_resource_view<decorated_context>;

    template<std::derived_from<context_base> Context>
    [[nodiscard]]
    GLuint get_index(generic_contextual_resource_view<Context> crv) { return crv.handle().index(); }
}