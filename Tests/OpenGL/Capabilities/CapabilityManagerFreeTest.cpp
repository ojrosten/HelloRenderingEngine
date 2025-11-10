////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CapabilityManagerFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Utilities/Casts.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"
#include "sequoia/Core/Meta/Utilities.hpp"
#include "sequoia/TestFramework/StateTransitionUtilities.hpp"

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

        throw std::runtime_error{std::format("Unable to convert gl_capability::{} to a string", to_gl_enum(cap))};
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

    enum class blend_eqn_mode : GLenum {
        add               = GL_FUNC_ADD,
        subtract          = GL_FUNC_SUBTRACT,
        reverse_substract = GL_FUNC_REVERSE_SUBTRACT,
        min               = GL_MIN,
        max               = GL_MAX
    };

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
}


namespace avocet::opengl {
    template<gl_capability Cap>
    struct capability_common_lifecycle {
        constexpr static auto capability{Cap};

        static void enable(const decorated_context& ctx) {
            gl_function{&GladGLContext::Enable}(ctx, to_gl_enum(capability));
        }

        static void disable(const decorated_context& ctx) {
            gl_function{&GladGLContext::Disable}(ctx, to_gl_enum(capability));
        }

        [[nodiscard]]
        friend constexpr bool operator==(const capability_common_lifecycle&, const capability_common_lifecycle&) noexcept = default;
    };

    namespace capabilities {
        struct gl_multi_sample : capability_common_lifecycle<gl_capability::multi_sample> {
            void configure(this const gl_multi_sample&, const decorated_context&) {}

            [[nodiscard]]
            friend constexpr bool operator==(const gl_multi_sample&, const gl_multi_sample&) noexcept = default;
        };

        struct gl_sample_coverage : capability_common_lifecycle<gl_capability::sample_coverage> {
            sample_coverage_value coverage_val{1.0};
            invert_sample_mask    mask{invert_sample_mask::no};

            void configure(this const gl_sample_coverage& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::SampleCoverage}(ctx, self.coverage_val.raw_value(), static_cast<GLboolean>(self.mask));
            }

            [[nodiscard]]
            friend constexpr bool operator==(const gl_sample_coverage&, const gl_sample_coverage&) noexcept = default;
        };

        struct blend_modes {
            blend_mode      source{blend_mode::one},
                       destination{blend_mode::zero};

            [[nodiscard]]
            friend constexpr bool operator==(const blend_modes&, const blend_modes&) noexcept = default;
        };

        struct gl_blend_data {
            blend_modes modes{};
            blend_eqn_mode algebraic_op{GL_FUNC_ADD};

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend_data&, const gl_blend_data&) noexcept = default;
        };

        struct gl_blend : capability_common_lifecycle<gl_capability::blend> {
            gl_blend_data rgb{}, alpha{rgb};
            std::array<GLfloat, 4> colour{};

            void configure(this const gl_blend& self, const decorated_context& ctx) {
                gl_function{&GladGLContext::BlendFuncSeparate}(
                    ctx,
                    to_gl_enum(self.rgb.modes.source),
                    to_gl_enum(self.rgb.modes.destination),
                    to_gl_enum(self.alpha.modes.source),
                    to_gl_enum(self.alpha.modes.destination)
                );

                gl_function{&GladGLContext::BlendEquationSeparate}(ctx, to_gl_enum(self.rgb.algebraic_op), to_gl_enum(self.alpha.algebraic_op));

                const auto& col{self.colour};
                gl_function{&GladGLContext::BlendColor}(ctx, col[0], col[1], col[2], col[3]);
            }

            [[nodiscard]]
            friend constexpr bool operator==(const gl_blend&, const gl_blend&) noexcept = default;
        };
    }

    class capability_manager {
        template<class T>
        struct toggled_capability {
            T state{};
            bool is_enabled{};
        };

        using payload_type
            = std::tuple<
                  toggled_capability<capabilities::gl_blend>,
                  toggled_capability<capabilities::gl_multi_sample>,
                  toggled_capability<capabilities::gl_sample_coverage>
              >;

        payload_type m_Payload;

        const decorated_context& m_Context;
    public:
        explicit capability_manager(const decorated_context& ctx)
            : m_Context{ctx}
        {
            capabilities::gl_multi_sample::disable(m_Context);
        }

        template<class... RequestedCaps>
        void new_payload(const std::tuple<RequestedCaps...>& requestedCaps) {
            auto update{
                [&] <class Cap> (toggled_capability<Cap>& cap) {
                    constexpr auto index{sequoia::meta::find_v<std::tuple<RequestedCaps...>, Cap>};
                    if constexpr(index >= sizeof...(RequestedCaps)) {
                        if(cap.is_enabled) {
                            Cap::disable(m_Context);
                            cap.is_enabled = false;
                        }
                    }
                    else {
                        if(!cap.is_enabled) {
                            Cap::enable(m_Context);
                            cap.is_enabled = true;
                        }

                        if(const auto& requested{std::get<index>(requestedCaps)}; requested != cap.state) {
                            cap.state = requested;
                            cap.state.configure(m_Context);
                        }
                    }
                }
            };

            sequoia::meta::for_each(m_Payload, update);
        }
    };
}

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    namespace {
        [[nodiscard]]
        GLint get_int_param(const agl::decorated_context& ctx, GLenum name) {
            GLint param{};
            agl::gl_function{&GladGLContext::GetIntegerv}(ctx, name, &param);
            return param;
        }
    }

    [[nodiscard]]
    std::filesystem::path capability_manager_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void capability_manager_free_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
        const auto& ctx{w.context()};

        check("Multisampling enabled by default",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("Blending disabled by default",     !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        namespace agl = avocet::opengl;
        agl::capability_manager capManager{ctx};

        check("Multisampling disabled by manager", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("",                                  !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(std::tuple{agl::capabilities::gl_blend{}});
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
        check(equality, "", get_int_param(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_ONE));
        check(equality, "", get_int_param(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ZERO));

        capManager.new_payload(
            std::tuple{
                agl::capabilities::gl_blend{
                    .rgb{  .modes{.source{agl::blend_mode::src_alpha}, .destination{agl::blend_mode::one_minus_src_alpha}}, .algebraic_op{GL_FUNC_ADD}},
                    .alpha{.modes{.source{agl::blend_mode::src_alpha}, .destination{agl::blend_mode::one_minus_src_alpha}}, .algebraic_op{GL_FUNC_ADD}},
                    .colour{}
                }
            }
        );
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
        check(equality, "", get_int_param(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_SRC_ALPHA));
        check(equality, "", get_int_param(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ONE_MINUS_SRC_ALPHA));

        capManager.new_payload(std::tuple{agl::capabilities::gl_multi_sample{}});
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(std::tuple{});
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(std::tuple{agl::capabilities::gl_multi_sample{}});
        check("",  agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
    }
}
