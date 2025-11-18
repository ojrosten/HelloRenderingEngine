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
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include "sequoia/TestFramework/FreeCheckers.hpp"

namespace avocet::opengl::testing {
    template<class T>
    [[nodiscard]]
    T get_int_param_as(const opengl::decorated_context& ctx, GLenum name) {
        GLint param{};
        opengl::gl_function{&GladGLContext::GetIntegerv}(ctx, name, &param);
        return static_cast<T>(param);
    }

    [[nodiscard]]
    GLboolean get_bool_param(const opengl::decorated_context& ctx, GLenum name) {
        GLboolean param{};
        opengl::gl_function{&GladGLContext::GetBooleanv}(ctx, name, &param);
        return param;
    }

    [[nodiscard]]
    GLfloat get_float_param(const opengl::decorated_context& ctx, GLenum name) {
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
            check(equality, "Source",      logger, obtained.source,      predicted.source);
            check(equality, "Destination", logger, obtained.destination, predicted.destination);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_blend_data> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_blend_data& obtained, const agl::capabilities::gl_blend_data& predicted) {
            check(equality, "Modes",       logger, obtained.modes,         predicted.modes);
            check(equality, "Algebraic Op", logger, obtained.algebraic_op, predicted.algebraic_op);
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_blend> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_blend& obtained, const agl::capabilities::gl_blend& predicted) {
            check(equality, "RGB",    logger, obtained.rgb,    predicted.rgb);
            check(equality, "Alpha",  logger, obtained.alpha,  predicted.alpha);
            check(equality, "Colour", logger, obtained.colour, predicted.colour);
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_blend& cpuCap, const agl::decorated_context& ctx) {
            using namespace avocet::opengl::testing;
            check(equality, "Source rgb   GPU/CPU",      logger, agl::to_gl_enum(cpuCap.rgb  .modes.source),      get_int_param_as<GLenum>(ctx, GL_BLEND_SRC_RGB));
            check(equality, "Source alpha GPU/CPU",      logger, agl::to_gl_enum(cpuCap.alpha.modes.source),      get_int_param_as<GLenum>(ctx, GL_BLEND_SRC_ALPHA));
            check(equality, "Destination rgb   GPU/CPU", logger, agl::to_gl_enum(cpuCap.rgb  .modes.destination), get_int_param_as<GLenum>(ctx, GL_BLEND_DST_RGB));
            check(equality, "Destination alpha GPU/CPU", logger, agl::to_gl_enum(cpuCap.alpha.modes.destination), get_int_param_as<GLenum>(ctx, GL_BLEND_DST_ALPHA));
            check(equality, "Blend equation GPU/CPU",    logger, agl::to_gl_enum(cpuCap.rgb  .algebraic_op),      get_int_param_as<GLenum>(ctx, GL_BLEND_EQUATION_RGB));
            check(equality, "Blend equation GPU/CPU",    logger, agl::to_gl_enum(cpuCap.alpha.algebraic_op),      get_int_param_as<GLenum>(ctx, GL_BLEND_EQUATION_ALPHA));

            check(equality, "Colour GPU/CPU", logger, cpuCap.colour, get_float_params<4>(ctx, GL_BLEND_COLOR));
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_sample_coverage> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_sample_coverage& obtained, const agl::capabilities::gl_sample_coverage& predicted) {
            check(equality, "Coverage",    logger, obtained.coverage_val,            predicted.coverage_val);
            check(equality, "Invert Mask", logger, agl::to_gl_bool(obtained.invert), agl::to_gl_bool(predicted.invert));
        }

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>& logger, const agl::capabilities::gl_sample_coverage& obtained, const agl::decorated_context& ctx) {
            using namespace avocet::opengl::testing;
            check(equality, "Coverage",      logger, get_float_param(ctx, GL_SAMPLE_COVERAGE_VALUE),  obtained.coverage_val.raw_value());
            check(equality, "Invert Mask",   logger, get_bool_param (ctx, GL_SAMPLE_COVERAGE_INVERT), static_cast<GLboolean>(obtained.invert));
        }
    };

    template<>
    struct value_tester<avocet::opengl::capabilities::gl_multi_sample> {
        template<test_mode Mode>
        static void test(equality_check_t, test_logger<Mode>&, const agl::capabilities::gl_multi_sample&, const agl::capabilities::gl_multi_sample&) {}

        template<test_mode Mode>
        static void test(weak_equivalence_check_t, test_logger<Mode>&, const agl::capabilities::gl_multi_sample&, const agl::decorated_context&) {}
    };
}