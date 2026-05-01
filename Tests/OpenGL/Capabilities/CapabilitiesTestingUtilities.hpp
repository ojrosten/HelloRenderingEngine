////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "avocet/OpenGL/Capabilities/Capabilities.hpp"
#include "avocet/OpenGL/Context/GLGetters.hpp"
#include "avocet/OpenGL/StateAwareContext/CapableContext.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include "sequoia/Core/Meta/Utilities.hpp"
#include "sequoia/TestFramework/FreeCheckers.hpp"

namespace avocet::opengl::testing {
    template<class T>
    [[nodiscard]]
    T get_int_param_as(const opengl::decorated_context& ctx, int_names name) {
        return static_cast<T>(get(ctx, name));
    }
}

namespace sequoia::testing
{
    namespace agl = avocet::opengl;

    template<class Cap>
        requires avocet::opengl::is_capability_v<Cap> && std::is_empty_v<Cap>
    struct value_tester<Cap> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const Cap&, const Cap&) {}
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_blend_modes> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_blend_modes& obtained, const agl::capabilities::gl_blend_modes& predicted) {
            check(equality, "Source"     , logger, obtained.source     , predicted.source);
            check(equality, "Destination", logger, obtained.destination, predicted.destination);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_blend_data> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_blend_data& obtained, const agl::capabilities::gl_blend_data& predicted) {
            check(equality, "Modes"      ,  logger, obtained.modes       , predicted.modes);
            check(equality, "Algebraic Op", logger, obtained.algebraic_op, predicted.algebraic_op);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_blend> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_blend& obtained, const agl::capabilities::gl_blend& predicted) {
            check(equality, "RGB"   , logger, obtained.rgb   , predicted.rgb);
            check(equality, "Alpha" , logger, obtained.alpha , predicted.alpha);
            check(equality, "Colour", logger, obtained.colour, predicted.colour);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_sample_coverage> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_sample_coverage& obtained, const agl::capabilities::gl_sample_coverage& predicted) {
            check(equality, "Coverage"   , logger, obtained.coverage_val,  predicted.coverage_val);
            check(equality, "Invert Mask", logger, obtained.invert      ,  predicted.invert);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_sample_alpha_to_coverage> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const agl::capabilities::gl_sample_alpha_to_coverage&, const agl::capabilities::gl_sample_alpha_to_coverage&) {}
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_stencil_func> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_stencil_func& obtained, const agl::capabilities::gl_stencil_func& predicted) {
            check(equality, "Comparison"     , logger, obtained.comparison     , predicted.comparison     );
            check(equality, "Reference Value", logger, obtained.reference_value, predicted.reference_value);
            check(equality, "Mask"           , logger, obtained.mask           , predicted.mask           );
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_stencil_op> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_stencil_op& obtained, const agl::capabilities::gl_stencil_op& predicted) {
            check(equality, "On Failure"                   , logger, obtained.on_failure                   , predicted.on_failure                   );
            check(equality, "On Pass with depth failure"   , logger, obtained.on_pass_with_depth_failure   , predicted.on_pass_with_depth_failure   );
            check(equality, "On Pass without depth failure", logger, obtained.on_pass_without_depth_failure, predicted.on_pass_without_depth_failure);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_stencil_write_mask> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_stencil_write_mask& obtained, const agl::capabilities::gl_stencil_write_mask& predicted) {
            check(equality, "Value", logger, obtained.mask, predicted.mask);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_stencil_test_separate> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_stencil_test_separate& obtained, const agl::capabilities::gl_stencil_test_separate& predicted) {
            check(equality, "Func",       logger, obtained.func,       predicted.func      );
            check(equality, "Op"  ,       logger, obtained.op  ,       predicted.op        );
            check(equality, "Write Mask", logger, obtained.write_mask, predicted.write_mask);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_stencil_test> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_stencil_test& obtained, const agl::capabilities::gl_stencil_test& predicted) {
            check(equality, "Front", logger, obtained.front, predicted.front);
            check(equality, "Back" , logger, obtained.back , predicted.back );
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_polygon_offset> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_polygon_offset& obtained, const agl::capabilities::gl_polygon_offset& predicted) {
            check(equality, "Factor", logger, obtained.factor, predicted.factor);
            check(equality, "Units",  logger, obtained.units , predicted.units);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_depth_test> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_depth_test& obtained, const agl::capabilities::gl_depth_test& predicted) {
            check(equality, "Func", logger, obtained.func, predicted.func);
            check(equality, "Mask", logger, obtained.mask, predicted.mask);
            check(equality, "Poly Offset", logger, obtained.poly_offset, predicted.poly_offset);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capable_context> {
        using payload_type = agl::capable_context::payload_type;

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const payload_type& payload) {
            auto checkGPUState{
                    [&] <class Cap> (const std::optional<Cap>&cap) {
                        testing::check(
                            equality,
                            std::format("{} is enabled", Cap::capability),
                            logger,
                            static_cast<bool>(agl::gl_function{&GladGLContext::IsEnabled}(ctx, to_gl_enum(Cap::capability))),
                            static_cast<bool>(cap)
                        );

                        if(cap)
                            testing::check(weak_equivalence, "GPU State", logger, ctx, cap.value());
                    }
            };

            sequoia::meta::for_each(payload, checkGPUState);
        }

        template<test_mode Mode, class T>
            requires avocet::opengl::is_capability_v<T>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const agl::capable_context&, const T&) {}


        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_blend& predicted) {
            using namespace avocet::opengl::testing;
            check(equality, "Source rgb   GPU/CPU"     , logger, get_int_param_as<GLenum>(ctx, agl::int_names::blend_src_rgb)       , agl::to_gl_enum(predicted.rgb.modes.source));
            check(equality, "Source alpha GPU/CPU"     , logger, get_int_param_as<GLenum>(ctx, agl::int_names::blend_src_alpha)     , agl::to_gl_enum(predicted.alpha.modes.source));
            check(equality, "Destination rgb   GPU/CPU", logger, get_int_param_as<GLenum>(ctx, agl::int_names::blend_dst_rgb)       , agl::to_gl_enum(predicted.rgb.modes.destination));
            check(equality, "Destination alpha GPU/CPU", logger, get_int_param_as<GLenum>(ctx, agl::int_names::blend_dst_alpha)     , agl::to_gl_enum(predicted.alpha.modes.destination));
            check(equality, "Blend equation GPU/CPU"   , logger, get_int_param_as<GLenum>(ctx, agl::int_names::blend_equation_rgb)  , agl::to_gl_enum(predicted.rgb.algebraic_op));
            check(equality, "Blend equation GPU/CPU"   , logger, get_int_param_as<GLenum>(ctx, agl::int_names::blend_equation_alpha), agl::to_gl_enum(predicted.alpha.algebraic_op));

            check(equality, "Colour GPU/CPU", logger, agl::get<GLfloat>(ctx, agl::quadruple_floating_point_names::blend_color), predicted.colour);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_polygon_offset& predicted) {
            using namespace avocet::opengl::testing;
            check(equality, "Factor", logger, agl::get<GLfloat>(ctx, agl::floating_point_names::polygon_offset_factor), predicted.factor);
            check(equality, "Units" , logger, agl::get<GLfloat>(ctx, agl::floating_point_names::polygon_offset_units) , predicted.units);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_depth_test& predicted) {
            using namespace avocet::opengl::testing;
            check(equality, "Func"       , logger, get_int_param_as<GLenum>(ctx, agl::int_names::depth_func), agl::to_gl_enum(predicted.func));
            check(equality, "MasK"       , logger, agl::get(ctx, agl::bool_names::depth_writemask)          , agl::to_gl_boolean(predicted.mask));
            check(weak_equivalence, "Poly Offset", logger, ctx, predicted.poly_offset);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_sample_coverage& predicted) {
            using namespace avocet::opengl::testing;
            check(equality, "Coverage"   , logger, agl::get<GLfloat>(ctx, agl::floating_point_names::sample_coverage_value), predicted.coverage_val.raw_value());
            check(equality, "Invert Mask", logger, agl::get         (ctx, agl::bool_names::sample_coverage_invert)         , agl::to_gl_boolean(predicted.invert));
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_stencil_test& predicted) {
            using namespace avocet::opengl::testing;
            using namespace avocet::opengl;
            check(equality, "Front Func"    , logger, get_int_param_as<comparison_mode>(ctx, agl::int_names::stencil_func), predicted.front.func.comparison     );
            check(equality, "Front Ref Val" , logger, get_int_param_as<GLint>          (ctx, agl::int_names::stencil_ref ), predicted.front.func.reference_value);
            check_mask("Front Val Mask", logger, ctx, agl::int_names::stencil_value_mask, predicted.front.func.mask);

            check(equality, "Front On Failure"                   , logger, get_int_param_as<stencil_failure_mode>(ctx, agl::int_names::stencil_fail           ), predicted.front.op.on_failure                   );
            check(equality, "Front On Pass with Depth Failure"   , logger, get_int_param_as<stencil_failure_mode>(ctx, agl::int_names::stencil_pass_depth_fail), predicted.front.op.on_pass_with_depth_failure   );
            check(equality, "Front On Pass without Depth Failure", logger, get_int_param_as<stencil_failure_mode>(ctx, agl::int_names::stencil_pass_depth_pass), predicted.front.op.on_pass_without_depth_failure);

            check_mask("Front Write Mask", logger, ctx, agl::int_names::stencil_writemask, predicted.front.write_mask.mask);

            check(equality, "Back Func"    , logger, get_int_param_as<comparison_mode>(ctx, agl::int_names::stencil_back_func), predicted.back.func.comparison     );
            check(equality, "Back Ref Val" , logger, get_int_param_as<GLint>          (ctx, agl::int_names::stencil_back_ref ), predicted.back.func.reference_value);
            check_mask("Back Val Mask", logger, ctx, agl::int_names::stencil_back_value_mask, predicted.back.func.mask);

            check(equality, "Back On Failure"                   , logger, get_int_param_as<stencil_failure_mode>(ctx, agl::int_names::stencil_back_fail           ), predicted.back.op.on_failure                   );
            check(equality, "Back On Pass with Depth Failure"   , logger, get_int_param_as<stencil_failure_mode>(ctx, agl::int_names::stencil_back_pass_depth_fail), predicted.back.op.on_pass_with_depth_failure   );
            check(equality, "Back On Pass without Depth Failure", logger, get_int_param_as<stencil_failure_mode>(ctx, agl::int_names::stencil_back_pass_depth_pass), predicted.back.op.on_pass_without_depth_failure);

            check_mask("Back Write Mask", logger, ctx, agl::int_names::stencil_back_writemask, predicted.back.write_mask.mask);
        }
    private:
      template<test_mode Mode>
      static void check_mask(std::string description, test_logger<Mode>& logger, const agl::capable_context& ctx, agl::int_names name, GLuint predicted) {
          using namespace avocet::opengl::testing;
          // Different drivers extract masks somewhat differently. The type is GLuint, though in practice only
          // the first byte is used. For a mask of all 1s, some drivers report 255 whereas others report
          // numeric_limits<GLuint>::max()
          // To give platform-independent output, anything beyond the first byte of the mask is masked.
          // It's masks all the way down...
          check(equality, description, logger, get_int_param_as<GLuint>(ctx, name) & 0xFF, predicted & 0xFF);
      }
    };
}