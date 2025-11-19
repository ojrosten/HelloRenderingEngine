////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Capabilities/CapabilitiesConfiguration.hpp"
#include "avocet/OpenGL/DecoratedContext/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

namespace avocet::opengl::capabilities::impl {
    void configure(const decorated_context& ctx, const gl_blend& current, const gl_blend& requested) {
        if((requested.rgb.modes != current.rgb.modes) or (requested.alpha.modes != current.alpha.modes)) {
            gl_function{&GladGLContext::BlendFuncSeparate}(
                ctx,
                to_gl_enum(requested.rgb.modes.source),
                to_gl_enum(requested.rgb.modes.destination),
                to_gl_enum(requested.alpha.modes.source),
                to_gl_enum(requested.alpha.modes.destination)
            );
        }

        if((requested.rgb.algebraic_op != current.rgb.algebraic_op) or (requested.alpha.algebraic_op != current.alpha.algebraic_op))
            gl_function{&GladGLContext::BlendEquationSeparate}(ctx, to_gl_enum(requested.rgb.algebraic_op), to_gl_enum(requested.alpha.algebraic_op));

        if(requested.colour != current.colour) {
            const auto& col{requested.colour};
            gl_function{&GladGLContext::BlendColor}(ctx, col[0], col[1], col[2], col[3]);
        }
    }

    void configure(const decorated_context& ctx, const gl_sample_coverage& current, const gl_sample_coverage& requested) {
        if(requested != current)
            gl_function{&GladGLContext::SampleCoverage}(ctx, requested.coverage_val.raw_value(), static_cast<GLboolean>(requested.invert));
    }
}