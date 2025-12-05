////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Capabilities/Capabilities.hpp"

namespace avocet::opengl {
    class decorated_context;
}

/// The use of the impl namespace is a tempory hack to discourage
/// clients of avocet from using these functions. Upon migration
/// to modules, the impl namespace will likely disappear
namespace avocet::opengl::capabilities::impl {
    template<class Cap>
        requires is_capability_v<Cap> && std::is_empty_v<Cap>
    void configure(const decorated_context&, const Cap&, const Cap&) {}

    void configure(const decorated_context& ctx, const gl_blend& current, const gl_blend& requested);

    void configure(const decorated_context& ctx, const gl_sample_coverage& current, const gl_sample_coverage& requested);

    void configure(const decorated_context& ctx, const gl_stencil_test& current, const gl_stencil_test& requested);

    void configure(const decorated_context& ctx, const gl_depth_test& current, const gl_depth_test& requested);

    void compensate_for_driver_init_bugs(const decorated_context& ctx, const gl_stencil_test& init);
}