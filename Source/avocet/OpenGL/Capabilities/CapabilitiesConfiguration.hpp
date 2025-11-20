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
    void configure(const decorated_context& ctx, const gl_blend& current, const gl_blend& requested);

    inline void configure(const decorated_context&, const gl_multi_sample&, const gl_multi_sample&) {}

    void configure(const decorated_context& ctx, const gl_sample_coverage& current, const gl_sample_coverage& requested);

    inline void configure(const decorated_context&, const gl_sample_alpha_to_coverage&, const gl_sample_alpha_to_coverage&) {}

    void configure(const decorated_context& ctx, const gl_stencil_test& current, const gl_stencil_test& requested);
}