////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Capabilities/CapabilitiesConfiguration.hpp"
#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Context/Version.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

namespace avocet::opengl::capabilities::impl {
    namespace {
        void do_configure(const decorated_context& ctx, face_selection_mode face, const gl_stencil_func& requested) {
            gl_function{&GladGLContext::StencilFuncSeparate}(ctx, to_gl_enum(face), to_gl_enum(requested.comparison), requested.reference_value, requested.mask);
        }

        void do_configure(const decorated_context& ctx, face_selection_mode face, const gl_stencil_op& requested) {
            gl_function{&GladGLContext::StencilOpSeparate}(ctx, to_gl_enum(face), to_gl_enum(requested.on_failure), to_gl_enum(requested.on_pass_with_depth_failure), to_gl_enum(requested.on_pass_without_depth_failure));
        }

        void do_configure(const decorated_context& ctx, face_selection_mode face, const gl_stencil_write_mask& requested) {
            gl_function{&GladGLContext::StencilMaskSeparate}(ctx, to_gl_enum(face), requested.mask);
        }

        template<class T>
        void dispatch_stencil_config(const decorated_context& ctx, const T& currentFront, const T& currentBack, const T& requestedFront, const T& requestedBack) {
            if((requestedFront == requestedBack) and (requestedFront != currentFront) and (requestedBack != currentBack)) {
                do_configure(ctx, face_selection_mode::front_and_back, requestedFront);
            }
            else {
                if(requestedFront != currentFront) {
                    do_configure(ctx, face_selection_mode::front, requestedFront);
                }

                if(requestedBack != currentBack) {
                    do_configure(ctx, face_selection_mode::back, requestedBack);
                }
            }
        }

        [[nodiscard]]
        bool is_intel_arc(std::string_view renderer) { return renderer.find("Intel(R) Arc") != std::string::npos; }
    }

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
            gl_function{&GladGLContext::SampleCoverage}(ctx, requested.coverage_val.raw_value(), to_gl_boolean(requested.invert));
    }

    void configure(const decorated_context& ctx, const gl_stencil_test& current, const gl_stencil_test& requested) {
        dispatch_stencil_config(ctx, current.front.func      , current.back.func      , requested.front.func      , requested.back.func      );
        dispatch_stencil_config(ctx, current.front.op        , current.back.op        , requested.front.op        , requested.back.op        );
        dispatch_stencil_config(ctx, current.front.write_mask, current.back.write_mask, requested.front.write_mask, requested.back.write_mask);
    }

    void configure(const decorated_context& ctx, const gl_depth_test& current, const gl_depth_test& requested) {
        if(requested.func != current.func)
            gl_function{&GladGLContext::DepthFunc}(ctx, to_gl_enum(requested.func));

        if(requested.mask != current.mask)
            gl_function{&GladGLContext::DepthMask}(ctx, to_gl_boolean(requested.mask));

        if(requested.poly_offset != current.poly_offset)
            gl_function{&GladGLContext::PolygonOffset}(ctx, requested.poly_offset.factor, requested.poly_offset.units);
    }

    void compensate_for_driver_init_bugs(const decorated_context& ctx, const gl_stencil_test& init) {
        if(is_intel_arc(get_renderer(ctx))) {
            const auto& func{init.front.func};
            gl_function{&GladGLContext::StencilFunc}(ctx, to_gl_enum(func.comparison), func.reference_value, func.mask);
        }
    }
}