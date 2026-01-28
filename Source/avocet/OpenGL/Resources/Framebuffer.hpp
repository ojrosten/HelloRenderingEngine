////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Resources/Textures.hpp"

namespace avocet::opengl {
    struct framebuffer_lifecycle_events {
        struct configurator {
            optional_label label;
        };

        constexpr static auto identifier{object_identifier::framebuffer};

        template<std::size_t N>
        static void generate(const decorated_context& ctx, raw_indices<N>& indices) { gl_function{&GladGLContext::GenFramebuffers}(ctx, N, indices.data()); }

        template<std::size_t N>
        static void destroy(const decorated_context& ctx, const raw_indices<N>& indices) { gl_function{&GladGLContext::DeleteFramebuffers}(ctx, N, indices.data()); }

        static void bind(contextual_resource_view h) { gl_function{&GladGLContext::BindFramebuffer}(h.context(), GL_FRAMEBUFFER, to_gl_enum(Species), get_index(h)); }

        static void configure(contextual_resource_view h, const configurator& config) {
            add_label(identifier, h, config.label);
        }
    };

    class framebuffer : public generic_resource<num_resources{1}, framebuffer_lifecycle_events> {

    };
}