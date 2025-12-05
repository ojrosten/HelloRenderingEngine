////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "avocet/OpenGL/Capabilities/Capabilities.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include "sequoia/Core/Meta/Utilities.hpp"
#include "sequoia/TestFramework/FreeCheckers.hpp"

namespace avocet::opengl::testing {
    template<class T>
    [[nodiscard]]
    T get_int_param_as(const opengl::decorated_context& ctx, GLenum name) {
        GLint param{};
        opengl::gl_function{&GladGLContext::GetIntegerv}(ctx, name, &param);
        return static_cast<T>(param);
    }

    template<class T>
    [[nodiscard]]
    T get_int64_param_as(const opengl::decorated_context& ctx, GLenum name) {
        GLint64 param{};
        opengl::gl_function{&GladGLContext::GetInteger64v}(ctx, name, &param);
        return static_cast<T>(param);
    }

    [[nodiscard]]
    inline GLboolean get_bool_param(const opengl::decorated_context& ctx, GLenum name) {
        GLboolean param{};
        opengl::gl_function{&GladGLContext::GetBooleanv}(ctx, name, &param);
        return param;
    }

    [[nodiscard]]
    inline GLfloat get_float_param(const opengl::decorated_context& ctx, GLenum name) {
        GLfloat param{};
        opengl::gl_function{&GladGLContext::GetFloatv}(ctx, name, &param);
        return param;
    }

    template<std::size_t N>
    [[nodiscard]]
    std::array<GLfloat, N> get_float_params(const opengl::decorated_context& ctx, GLenum name) {
        std::array<GLfloat, N> params{};
        opengl::gl_function{&GladGLContext::GetFloatv}(ctx, name, params.data());
        return params;
    }
}

namespace sequoia::testing
{
    namespace agl = avocet::opengl;

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
    struct value_tester<avocet::opengl::capabilities::gl_multi_sample> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const agl::capabilities::gl_multi_sample&, const agl::capabilities::gl_multi_sample&) {}
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
            check(equality, "Source rgb   GPU/CPU"     , logger, get_int_param_as<GLenum>(ctx, GL_BLEND_SRC_RGB)       , agl::to_gl_enum(predicted.rgb.modes.source));
            check(equality, "Source alpha GPU/CPU"     , logger, get_int_param_as<GLenum>(ctx, GL_BLEND_SRC_ALPHA)     , agl::to_gl_enum(predicted.alpha.modes.source));
            check(equality, "Destination rgb   GPU/CPU", logger, get_int_param_as<GLenum>(ctx, GL_BLEND_DST_RGB)       , agl::to_gl_enum(predicted.rgb.modes.destination));
            check(equality, "Destination alpha GPU/CPU", logger, get_int_param_as<GLenum>(ctx, GL_BLEND_DST_ALPHA)     , agl::to_gl_enum(predicted.alpha.modes.destination));
            check(equality, "Blend equation GPU/CPU"   , logger, get_int_param_as<GLenum>(ctx, GL_BLEND_EQUATION_RGB)  , agl::to_gl_enum(predicted.rgb.algebraic_op));
            check(equality, "Blend equation GPU/CPU"   , logger, get_int_param_as<GLenum>(ctx, GL_BLEND_EQUATION_ALPHA), agl::to_gl_enum(predicted.alpha.algebraic_op));

            check(equality, "Colour GPU/CPU", logger, get_float_params<4>(ctx, GL_BLEND_COLOR), predicted.colour);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_polygon_offset& predicted) {
            using namespace avocet::opengl::testing;
            check(equality, "Factor", logger, get_float_param(ctx, GL_POLYGON_OFFSET_FACTOR), predicted.factor);
            check(equality, "Units" , logger, get_float_param(ctx, GL_POLYGON_OFFSET_UNITS) , predicted.units);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_depth_test& predicted) {
            using namespace avocet::opengl::testing;
            check(equality, "Func"       , logger, get_int_param_as<GLenum>(ctx, GL_DEPTH_FUNC), agl::to_gl_enum(predicted.func));
            check(equality, "MasK"       , logger, get_bool_param(ctx, GL_DEPTH_WRITEMASK)     , agl::to_gl_boolean(predicted.mask));
            check(weak_equivalence, "Poly Offset", logger, ctx, predicted.poly_offset);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_sample_coverage& predicted) {
            using namespace avocet::opengl::testing;
            check(equality, "Coverage"   , logger, get_float_param(ctx, GL_SAMPLE_COVERAGE_VALUE), predicted.coverage_val.raw_value());
            check(equality, "Invert Mask", logger, get_bool_param(ctx, GL_SAMPLE_COVERAGE_INVERT), agl::to_gl_boolean(predicted.invert));
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capable_context& ctx, const agl::capabilities::gl_stencil_test& predicted) {
            using namespace avocet::opengl::testing;
            using namespace avocet::opengl;
            check(equality, "Front Func"    , logger, get_int_param_as<comparison_mode>(ctx, GL_STENCIL_FUNC), predicted.front.func.comparison     );
            check(equality, "Front Ref Val" , logger, get_int_param_as<GLint>          (ctx, GL_STENCIL_REF ), predicted.front.func.reference_value);
            check_mask("Front Val Mask", logger, ctx, GL_STENCIL_VALUE_MASK, predicted.front.func.mask);

            check(equality, "Front On Failure"                   , logger, get_int_param_as<stencil_failure_mode>(ctx, GL_STENCIL_FAIL           ), predicted.front.op.on_failure                   );
            check(equality, "Front On Pass with Depth Failure"   , logger, get_int_param_as<stencil_failure_mode>(ctx, GL_STENCIL_PASS_DEPTH_FAIL), predicted.front.op.on_pass_with_depth_failure   );
            check(equality, "Front On Pass without Depth Failure", logger, get_int_param_as<stencil_failure_mode>(ctx, GL_STENCIL_PASS_DEPTH_PASS), predicted.front.op.on_pass_without_depth_failure);

            check_mask("Front Write Mask", logger, ctx, GL_STENCIL_WRITEMASK, predicted.front.write_mask.mask);

            check(equality, "Back Func"    , logger, get_int_param_as<comparison_mode>(ctx, GL_STENCIL_BACK_FUNC), predicted.back.func.comparison     );
            check(equality, "Back Ref Val" , logger, get_int_param_as<GLint>          (ctx, GL_STENCIL_BACK_REF ), predicted.back.func.reference_value);
            check_mask("Back Val Mask", logger, ctx, GL_STENCIL_BACK_VALUE_MASK, predicted.back.func.mask);

            check(equality, "Back On Failure"                   , logger, get_int_param_as<stencil_failure_mode>(ctx, GL_STENCIL_BACK_FAIL           ), predicted.back.op.on_failure                   );
            check(equality, "Back On Pass with Depth Failure"   , logger, get_int_param_as<stencil_failure_mode>(ctx, GL_STENCIL_BACK_PASS_DEPTH_FAIL), predicted.back.op.on_pass_with_depth_failure   );
            check(equality, "Back On Pass without Depth Failure", logger, get_int_param_as<stencil_failure_mode>(ctx, GL_STENCIL_BACK_PASS_DEPTH_PASS), predicted.back.op.on_pass_without_depth_failure);

            check_mask("Back Write Mask", logger, ctx, GL_STENCIL_BACK_WRITEMASK, predicted.back.write_mask.mask);
        }
    private:
      template<test_mode Mode>
      static void check_mask(std::string description, test_logger<Mode>& logger, const agl::capable_context& ctx, GLenum name, GLuint predicted) {
          using namespace avocet::opengl::testing;
          // Different drivers extract masks somewhat differently. The type is GLuint, though in practice only
          // the first byte is used. For a mask of all 1s, some drivers report 255 whereas others report
          // numeric_limits<GLuint>::max()
          // To give platform-independent output, anything beyond the first byte of the mask is masked.
          // It's masks all the way down...
          check(equality, description, logger, get_int64_param_as<GLuint>(ctx, name) & 0xFF, predicted & 0xFF);
      }
    };
}