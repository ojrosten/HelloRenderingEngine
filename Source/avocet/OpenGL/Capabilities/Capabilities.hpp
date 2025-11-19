////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"

#include <array>
#include <format>

namespace avocet::opengl {
    enum class gl_capability : GLenum {
        blend                         = GL_BLEND,
        clip_distance_0               = GL_CLIP_DISTANCE0,
        clip_distance_1               = GL_CLIP_DISTANCE1,
        clip_distance_2               = GL_CLIP_DISTANCE2,
        colour_logic_op               = GL_COLOR_LOGIC_OP,
        cull_face                     = GL_CULL_FACE,
        debug_ouptut                  = GL_DEBUG_OUTPUT,
        debug_ouptut_synchronous      = GL_DEBUG_OUTPUT_SYNCHRONOUS,
        depth_clamp                   = GL_DEPTH_CLAMP,
        depth_test                    = GL_DEPTH_TEST,
        dither                        = GL_DITHER,
        framebuffer_srgb              = GL_FRAMEBUFFER_SRGB,
        line_smooth                   = GL_LINE_SMOOTH,
        multi_sample                  = GL_MULTISAMPLE,
        polygon_offset_fill           = GL_POLYGON_OFFSET_FILL,
        polygon_offset_line           = GL_POLYGON_OFFSET_LINE,
        polygon_offset_point          = GL_POLYGON_OFFSET_POINT,
        polygon_smooth                = GL_POLYGON_SMOOTH,
        primitive_restart             = GL_PRIMITIVE_RESTART,
        primitive_restart_fixed_index = GL_PRIMITIVE_RESTART_FIXED_INDEX,
        rasterizer_discard            = GL_RASTERIZER_DISCARD,
        sample_alpha_to_coverage      = GL_SAMPLE_ALPHA_TO_COVERAGE,
        sample_alpha_to_one           = GL_SAMPLE_ALPHA_TO_ONE,
        sample_coverage               = GL_SAMPLE_COVERAGE,
        sample_shading                = GL_SAMPLE_SHADING,
        sample_mask                   = GL_SAMPLE_MASK,
        scissor_test                  = GL_SCISSOR_TEST,
        stencil_test                  = GL_STENCIL_TEST,
        texture_cube_map_seamless     = GL_TEXTURE_CUBE_MAP_SEAMLESS,
        program_point_size            = GL_PROGRAM_POINT_SIZE
    };

    [[nodiscard]]
    std::string to_string(gl_capability cap);

    enum class blend_mode : GLenum {
        zero                   = GL_ZERO,
        one                    = GL_ONE,
        src_colour             = GL_SRC_COLOR,
        one_minus_src_colour   = GL_ONE_MINUS_SRC_COLOR,
        dst_colour             = GL_DST_COLOR,
        one_minus_dst_colour   = GL_ONE_MINUS_DST_COLOR,
        src_alpha              = GL_SRC_ALPHA,
        one_minus_src_alpha    = GL_ONE_MINUS_SRC_ALPHA,
        dst_alpha              = GL_DST_ALPHA,
        one_minus_dst_alpha    = GL_ONE_MINUS_DST_ALPHA,
        const_colour           = GL_CONSTANT_COLOR,
        one_minus_const_colour = GL_ONE_MINUS_CONSTANT_COLOR,
        const_alpha            = GL_CONSTANT_ALPHA,
        one_minus_const_alpha  = GL_ONE_MINUS_CONSTANT_ALPHA
    };

    [[nodiscard]]
    std::string to_string(blend_mode mode);

    enum class blend_eqn_mode : GLenum {
        add              = GL_FUNC_ADD,
        subtract         = GL_FUNC_SUBTRACT,
        reverse_subtract = GL_FUNC_REVERSE_SUBTRACT,
        min              = GL_MIN,
        max              = GL_MAX
    };

    [[nodiscard]]
    std::string to_string(blend_eqn_mode mode);

    class sample_coverage_value {
        GLfloat m_Value{1.0};
    public:
        constexpr sample_coverage_value() = default;

        constexpr explicit sample_coverage_value(GLfloat val)
            : m_Value{val}
        {
            if((m_Value < 0.0) || (m_Value > 1.0))
                throw std::domain_error{std::format("Coverage value of {} requested, but must be in the range [0,1]", m_Value)};
        }

        [[nodiscard]]
        constexpr GLfloat raw_value() const noexcept { return m_Value; }

        [[nodiscard]]
        constexpr friend auto operator<=>(const sample_coverage_value&, const sample_coverage_value&) noexcept = default;
    };

    enum class invert_sample_mask : GLboolean {
        no  = GL_FALSE,
        yes = GL_TRUE
    };

    namespace capabilities {
        struct gl_blend_modes {
            blend_mode      source{blend_mode::one},
                destination{blend_mode::zero};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend_modes&, const gl_blend_modes&) noexcept = default;
        };

        struct gl_blend_data {
            gl_blend_modes modes{};
            blend_eqn_mode algebraic_op{GL_FUNC_ADD};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend_data&, const gl_blend_data&) noexcept = default;
        };

        struct gl_blend {
            constexpr static auto capability{gl_capability::blend};
            gl_blend_data rgb{}, alpha{rgb};
            std::array<GLfloat, 4> colour{};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };

        struct gl_multi_sample {
            constexpr static auto capability{gl_capability::multi_sample};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_sample_coverage {
            constexpr static auto capability{gl_capability::sample_coverage};

            sample_coverage_value coverage_val{1.0};
            invert_sample_mask    invert{invert_sample_mask::no};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_coverage&, const gl_sample_coverage&) noexcept = default;
        };

        struct gl_sample_alpha_to_coverage {
            constexpr static auto capability{gl_capability::sample_alpha_to_coverage};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_alpha_to_coverage&, const gl_sample_alpha_to_coverage&) noexcept = default;
        };
    }
}

namespace std {
    template<>
    struct formatter<avocet::opengl::sample_coverage_value> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::opengl::sample_coverage_value coverage, auto& ctx) const {
            return format_to(ctx.out(), "{}", coverage.raw_value());
        }
    };

    template<>
    struct formatter<avocet::opengl::gl_capability> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::opengl::gl_capability cap, auto& ctx) const {
            return format_to(ctx.out(), "{}", to_string(cap));
        }
    };

    template<>
    struct formatter<avocet::opengl::blend_mode> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::opengl::blend_mode mode, auto& ctx) const {
            return format_to(ctx.out(), "{}", to_string(mode));
        }
    };

    template<>
    struct formatter<avocet::opengl::blend_eqn_mode> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::opengl::blend_eqn_mode mode, auto& ctx) const {
            return format_to(ctx.out(), "{}", to_string(mode));
        }
    };
}
