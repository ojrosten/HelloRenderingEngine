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


namespace avocet::opengl {
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

        namespace impl {
            void configure(const decorated_context& ctx, const gl_blend& requested, const gl_blend& current) {
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

            void configure(const decorated_context&, const gl_multi_sample&, const gl_multi_sample&) {}

            void configure(const decorated_context& ctx, const gl_sample_coverage& requested, const gl_sample_coverage& current) {
                if(requested != current)
                    gl_function{&GladGLContext::SampleCoverage}(ctx, requested.coverage_val.raw_value(), static_cast<GLboolean>(requested.invert));
            }
        }
    }

    class capability_manager {
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

        template<class T>
        struct toggled_capability {
            T state{};
            bool is_enabled{};
        };

        using toggled_payload_type
            = std::tuple<
                  toggled_capability<capabilities::gl_blend>,
                  toggled_capability<capabilities::gl_multi_sample>,
                  toggled_capability<capabilities::gl_sample_coverage>
              >;

        toggled_payload_type m_Payload;

        const decorated_context& m_Context;

        template<class Cap>
        void disable(toggled_capability<Cap>& cap) {
            if(cap.is_enabled) {
                capability_common_lifecycle<Cap::capability>::disable(m_Context);
                cap.is_enabled = false;
            }
        }

        template<class Cap>
        void enable(toggled_capability<Cap>& cap) {
            if(!cap.is_enabled) {
                capability_common_lifecycle<Cap::capability>::enable(m_Context);
                cap.is_enabled = true;
            }
        }

        template<class Cap>
        void update_config(toggled_capability<Cap>& current, const Cap& requested) {
            if(current.state != requested) {
                capabilities::impl::configure(m_Context, requested, current.state);
                current.state = requested;
            }
        }

        template<class Cap>
        void full_update(toggled_capability<Cap>& current, const std::optional<Cap>& requested) {
            if(!requested) {
                disable(current);
            }
            else {
                enable(current);
                update_config(current, requested.value());
            }
        }
    public:
        using payload_type
            = std::tuple<
                  std::optional<capabilities::gl_blend>,
                  std::optional<capabilities::gl_multi_sample>,
                  std::optional<capabilities::gl_sample_coverage>
              >;

        explicit capability_manager(const decorated_context& ctx)
            : m_Context{ctx}
        {
            capability_common_lifecycle<gl_capability::multi_sample>::disable(m_Context);
        }

        template<class... RequestedCaps>
        void new_payload(const std::tuple<RequestedCaps...>& requestedCaps) {
            auto update{
                [&] <class Cap> (toggled_capability<Cap>& cap) {
                    constexpr auto index{sequoia::meta::find_v<std::tuple<RequestedCaps...>, Cap>};
                    if constexpr(index >= sizeof...(RequestedCaps)) {
                        disable(cap);
                    }
                    else {
                        enable(cap);
                        update_config(cap, std::get<index>(requestedCaps));
                    }
                }
            };

            sequoia::meta::for_each(m_Payload, update);
        }

        void new_payload(const payload_type& payload) {
            [&, this] <std::size_t... Is>(std::index_sequence<Is...>) {
                (full_update(std::get<Is>(m_Payload), std::get<Is>(payload)), ...);
            }(std::make_index_sequence<std::tuple_size_v<toggled_payload_type>>{});
        }
    };
}

namespace {
    namespace agl = avocet::opengl;

    template<class T>
    [[nodiscard]]
    T get_int_param_as(const agl::decorated_context& ctx, GLenum name) {
        GLint param{};
        agl::gl_function{&GladGLContext::GetIntegerv}(ctx, name, &param);
        return static_cast<T>(param);
    }

    [[nodiscard]]
    GLboolean get_bool_param(const agl::decorated_context& ctx, GLenum name) {
        GLboolean param{};
        agl::gl_function{&GladGLContext::GetBooleanv}(ctx, name, &param);
        return param;
    }

    [[nodiscard]]
    GLfloat get_float_param(const agl::decorated_context& ctx, GLenum name) {
        GLfloat param{};
        agl::gl_function{&GladGLContext::GetFloatv}(ctx, name, &param);
        return param;
    }

    template<std::size_t N>
    [[nodiscard]]
    std::array<GLfloat, N> get_float_params(const agl::decorated_context& ctx, GLenum name) {
        std::array<GLfloat, N> params{};
        agl::gl_function{&GladGLContext::GetFloatv}(ctx, name, params.data());
        return params;
    }

    using payload_type = agl::capability_manager::payload_type;

    template<class... Caps>
    [[nodiscard]]
    payload_type make_payload(const Caps&... caps) {
        payload_type payload{};
        ((std::get<std::optional<Caps>>(payload) = caps), ...);
        return payload;
    }

    template<class... Caps>
    [[nodiscard]]
    payload_type set_payload(agl::capability_manager& capMan, const payload_type& hostPayload, const Caps&... targetCaps) {
        capMan.new_payload(hostPayload);
        const auto payload{make_payload(targetCaps...)};
        capMan.new_payload(payload);
        return payload;
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

namespace avocet::testing
{
    namespace agl = avocet::opengl;

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
        using namespace agl::capabilities;
        agl::capability_manager capManager{ctx};

        enum node_name {
            none            = 0,
            blend           = 1,
            multi_sample    = 2,
            sample_coverage = 3
        };

        using graph_type = transition_checker<payload_type>::transition_graph;

        graph_type graph{
            {
                {
                    {
                        node_name::none,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload); }
                    },
                    {
                        node_name::blend,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload, gl_blend{}); }
                    }
                },
                {
                    {
                        node_name::none,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload); }
                    }
                },
                {
                    {
                        node_name::none,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload); }
                    }
                },
                {
                    {
                        node_name::none,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload); }
                    }
                }
            },
            {
                payload_type{},
                make_payload(gl_blend{}),
                make_payload(gl_multi_sample{}),
                make_payload(gl_sample_coverage{})
            }
        };

        auto checker{
            [&ctx, this](std::string_view description, const payload_type& obtained, const payload_type& predicted) {
                check(equality, description, obtained, predicted);

                auto checkGPUState{
                    [&] <class Cap> (const std::optional<Cap>&cap) {
                        check(
                            equality,
                            std::format("{}\n{} is enabled", description, Cap::capability),
                            static_cast<bool>(agl::gl_function{&GladGLContext::IsEnabled}(ctx, to_gl_enum(Cap::capability))),
                            static_cast<bool>(cap)
                        );

                        if(cap)
                            check(weak_equivalence, std::format("{}\nGPU State", description), cap.value(), ctx);
                    }
                };

                sequoia::meta::for_each(predicted, checkGPUState);
            }
        };

        transition_checker<payload_type>::check("", graph, checker);

        check("Multisampling disabled by manager", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));

        capManager.new_payload(std::tuple{agl::capabilities::gl_blend{}});
        check("", !agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_MULTISAMPLE));
        check("", agl::gl_function{&GladGLContext::IsEnabled}(ctx, GL_BLEND));
        check(equality, "", get_int_param_as<GLint>(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_ONE));
        check(equality, "", get_int_param_as<GLint>(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ZERO));

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
        check(equality, "", get_int_param_as<GLint>(ctx, GL_BLEND_SRC_ALPHA), agl::to_gl_int(GL_SRC_ALPHA));
        check(equality, "", get_int_param_as<GLint>(ctx, GL_BLEND_DST_ALPHA), agl::to_gl_int(GL_ONE_MINUS_SRC_ALPHA));

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
