////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Formatting/Formatting.hpp"

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

    [[nodiscard]]
    std::string to_string(invert_sample_mask mode);

    enum class stencil_failure_mode : GLenum {
        keep           = GL_KEEP,
        zero           = GL_ZERO,
        replace        = GL_REPLACE,
        increment      = GL_INCR,
        increment_wrap = GL_INCR_WRAP,
        decrement      = GL_DECR,
        decrement_wrap = GL_DECR_WRAP,
        invert         = GL_INVERT
    };

    [[nodiscard]]
    std::string to_string(stencil_failure_mode mode);

    enum class comparison_mode : GLenum {
        never            = GL_NEVER,
        less             = GL_LESS,
        less_or_equal    = GL_LEQUAL,
        greater          = GL_GREATER,
        greater_or_equal = GL_GEQUAL,
        equal            = GL_EQUAL,
        not_equal        = GL_NOTEQUAL,
        always           = GL_ALWAYS
    };

    [[nodiscard]]
    std::string to_string(comparison_mode mode);

    enum class face_selection_mode : GLenum {
        front          = GL_FRONT,
        back           = GL_BACK,
        front_and_back = GL_FRONT_AND_BACK
    };

    [[nodiscard]]
    std::string to_string(face_selection_mode mode);

    enum class depth_buffer_write_mode : GLboolean {
        disabled = GL_FALSE,
        enabled  = GL_TRUE
    };

    [[nodiscard]]
    std::string to_string(depth_buffer_write_mode mode);

    template<class T>
    inline constexpr bool is_capability_v{
           std::regular<T>
        && requires {
              { T::capability } -> std::convertible_to<gl_capability>;
          }
    };

    namespace capabilities {
        /******************************* Blending *******************************/
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

        /******************************* Depth Testing *******************************/

        struct gl_polygon_offset {
            GLfloat factor{};
            GLfloat units{};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_offset&, const gl_polygon_offset&) noexcept = default;
        };

        struct gl_depth_test {
            constexpr static auto capability{gl_capability::depth_test};

            comparison_mode func{comparison_mode::less};
            depth_buffer_write_mode mask{depth_buffer_write_mode::enabled};
            gl_polygon_offset poly_offset{};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_depth_test&, const gl_depth_test&) noexcept = default;
        };

        /******************************* Multisampling *******************************/

        struct gl_multi_sample {
            constexpr static auto capability{gl_capability::multi_sample};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        /******************************* Polygon Offsetting *******************************/

        struct gl_polygon_offset_fill {
            constexpr static auto capability{gl_capability::polygon_offset_fill};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_offset_fill&, const gl_polygon_offset_fill&) noexcept = default;
        };

        struct gl_polygon_offset_line {
            constexpr static auto capability{gl_capability::polygon_offset_line};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_offset_line&, const gl_polygon_offset_line&) noexcept = default;
        };

        struct gl_polygon_offset_point {
            constexpr static auto capability{gl_capability::polygon_offset_point};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_polygon_offset_point&, const gl_polygon_offset_point&) noexcept = default;
        };

        /******************************* Sample Coverage *******************************/

        struct gl_sample_coverage {
            constexpr static auto capability{gl_capability::sample_coverage};

            sample_coverage_value coverage_val{1.0};
            invert_sample_mask    invert{invert_sample_mask::no};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_coverage&, const gl_sample_coverage&) noexcept = default;
        };

        /******************************* Sample Alpha to Coverage *******************************/

        struct gl_sample_alpha_to_coverage {
            constexpr static auto capability{gl_capability::sample_alpha_to_coverage};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_alpha_to_coverage&, const gl_sample_alpha_to_coverage&) noexcept = default;
        };

        /******************************* Stencil Testing *******************************/

        struct gl_stencil_func {
            comparison_mode comparison{comparison_mode::always};
            GLint reference_value{};
            GLuint mask{std::numeric_limits<GLuint>::max()};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_stencil_func&, const gl_stencil_func&) noexcept = default;
        };

        struct gl_stencil_op {
            stencil_failure_mode
                on_failure                   {stencil_failure_mode::keep},
                on_pass_with_depth_failure   {stencil_failure_mode::keep},
                on_pass_without_depth_failure{stencil_failure_mode::keep};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_stencil_op&, const gl_stencil_op&) noexcept = default;
        };

        struct gl_stencil_write_mask {
            GLuint mask{std::numeric_limits<GLuint>::max()};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_stencil_write_mask&, const gl_stencil_write_mask&) noexcept = default;
        };

        struct gl_stencil_test_separate {
            gl_stencil_func       func{};
            gl_stencil_op         op{};
            gl_stencil_write_mask write_mask{};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_stencil_test_separate&, const gl_stencil_test_separate&) noexcept = default;
        };

        struct gl_stencil_test {
            constexpr static auto capability{gl_capability::stencil_test};

            gl_stencil_test_separate front{}, back{};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_stencil_test&, const gl_stencil_test&) noexcept = default;
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
}
