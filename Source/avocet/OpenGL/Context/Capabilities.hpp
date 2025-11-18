////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "glad/gl.h"

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
    std::string to_string(gl_capability cap) {
        using enum gl_capability;
        switch(cap) {
        case blend:                         return "blend";
        case clip_distance_0:               return "clip_distance_0";
        case clip_distance_1:               return "clip_distance_1";
        case clip_distance_2:               return "clip_distance_2";
        case colour_logic_op:               return "colour_logic_op";
        case cull_face:                     return "cull_face";
        case debug_ouptut:                  return "debug_ouptut";
        case debug_ouptut_synchronous:      return "debug_ouptut_synchronous";
        case depth_clamp:                   return "depth_clamp";
        case depth_test:                    return "depth_test";
        case dither:                        return "dither";
        case framebuffer_srgb:              return "framebuffer_srgb";
        case line_smooth:                   return "line_smooth";
        case multi_sample:                  return "multi_sample";
        case polygon_offset_fill:           return "polygon_offset_fill";
        case polygon_offset_line:           return "polygon_offset_line";
        case polygon_offset_point:          return "polygon_offset_point";
        case polygon_smooth:                return "polygon_smooth";
        case primitive_restart:             return "primitive_restart";
        case primitive_restart_fixed_index: return "primitive_restart_fixed_index";
        case rasterizer_discard:            return "rasterizer_discard";
        case sample_alpha_to_coverage:      return "sample_alpha_to_coverage";
        case sample_alpha_to_one:           return "sample_alpha_to_one";
        case sample_coverage:               return "sample_coverage";
        case sample_shading:                return "sample_shading";
        case sample_mask:                   return "sample_mask";
        case scissor_test:                  return "scissor_test";
        case stencil_test:                  return "stencil_test";
        case texture_cube_map_seamless:     return "texture_cube_map_seamless";
        case program_point_size:            return "program_point_size";
        }

        throw std::runtime_error{std::format("Unrecognized option gl_capability::{} while stringifying", to_gl_enum(cap))};
    }

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
    std::string to_string(blend_mode mode) {
        using enum blend_mode;
        switch(mode)
        {
        case zero:                   return "zero";
        case one:                    return "one";
        case src_colour:             return "src_colour";
        case one_minus_src_colour:   return "one_minus_src_colour";
        case dst_colour:             return "dst_colour";
        case one_minus_dst_colour:   return "one_minus_dst_colour";
        case src_alpha:              return "src_alpha";
        case one_minus_src_alpha:    return "one_minus_src_alpha";
        case dst_alpha:              return "dst_alpha";
        case one_minus_dst_alpha:    return "one_minus_dst_alpha";
        case const_colour:           return "const_colour";
        case one_minus_const_colour: return "one_minus_const_colour";
        case const_alpha:            return "const_alpha";
        case one_minus_const_alpha:  return "one_minus_const_alpha ";
        }

        throw std::runtime_error{std::format("Unrecognized option blend_mode::{} while stringifying", to_gl_enum(mode))};
    }

    enum class blend_eqn_mode : GLenum {
        add              = GL_FUNC_ADD,
        subtract         = GL_FUNC_SUBTRACT,
        reverse_subtract = GL_FUNC_REVERSE_SUBTRACT,
        min              = GL_MIN,
        max              = GL_MAX
    };

    [[nodiscard]]
    std::string to_string(blend_eqn_mode mode) {
        using enum blend_eqn_mode;
        switch(mode)
        {
        case add:              return "add";
        case subtract:         return "subtract";
        case reverse_subtract: return "reverse_subtract";
        case min:              return "min";
        case max:              return "max";
        }

        throw std::runtime_error{std::format("Unrecognized option blend_eqn_mode::{} while stringifying", to_gl_enum(mode))};
    }

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
