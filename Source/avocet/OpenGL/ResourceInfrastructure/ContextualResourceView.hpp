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

    class contextual_resource_view {
        const decorated_context* m_Context{};
        const resource_handle* m_Handle{};
    public:
        contextual_resource_view(const decorated_context& ctx, const resource_handle& crv)
            : m_Context{&ctx}
            , m_Handle{&crv}
        {}

        [[nodiscard]]
        const decorated_context& context() const noexcept { return *m_Context; }

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return *m_Handle; }

        [[nodiscard]]
        friend bool operator==(const contextual_resource_view&, const contextual_resource_view&) noexcept = default;
    };

    [[nodiscard]]
    inline GLuint get_index(contextual_resource_view crv) { return crv.handle().index(); }
}