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
#include "avocet/OpenGL/Context/CapabilitiesConfiguration.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"
#include "sequoia/Core/Meta/Utilities.hpp"
#include "sequoia/TestFramework/StateTransitionUtilities.hpp"

#include <format>

namespace avocet::opengl {

    class capability_manager {
        template<class T>
        struct toggled_capability {
            T state{};
            bool is_enabled{(T::capability == gl_capability::dither) or (T::capability == gl_capability::multi_sample)};
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
                gl_function{&GladGLContext::Disable}(m_Context, to_gl_enum(Cap::capability));
                cap.is_enabled = false;
            }
        }

        template<class Cap>
        void enable(toggled_capability<Cap>& cap) {
            if(!cap.is_enabled) {
                gl_function{&GladGLContext::Enable}(m_Context, to_gl_enum(Cap::capability));
                cap.is_enabled = true;
            }
        }

        template<class Cap>
        void update_config(toggled_capability<Cap>& current, const Cap& requested) {
            if(current.state != requested) {
                capabilities::impl::configure(m_Context, current.state, requested);
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
            sequoia::meta::for_each(m_Payload, [this](auto& cap) { disable(cap); });
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
            none             = 0,
            blend            = 1,
            multi_sample     = 2,
            sample_coverage  = 3,
            configured_blend = 4
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
                    },
                    {
                        node_name::multi_sample,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload, gl_multi_sample{}); }
                    },
                    {
                        node_name::sample_coverage,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload, gl_sample_coverage{}); }
                    },
                    {
                        node_name::configured_blend,
                        report(""),
                        [&capManager](const payload_type& hostPayload) {
                            return
                                set_payload(
                                    capManager,
                                    hostPayload,
                                    gl_blend{
                                        .rgb  {.modes{.source{agl::blend_mode::src_colour}, .destination{agl::blend_mode::one_minus_src_colour}}, .algebraic_op{agl::blend_eqn_mode::subtract}},
                                        .alpha{.modes{.source{agl::blend_mode::dst_alpha},  .destination{agl::blend_mode::one_minus_dst_alpha}},  .algebraic_op{agl::blend_eqn_mode::reverse_subtract}},
                                        .colour{0.1f, 0.2f, 0.3f, 0.4f}
                                    }
                                );
                        }
                    },
                },
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
                    },
                    {
                        node_name::multi_sample,
                        report(""),
                        [&capManager](const payload_type& hostPayload) { return set_payload(capManager, hostPayload, gl_multi_sample{}); }
                    },
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
                },
            },
            {
                payload_type{},
                make_payload(gl_blend{}),
                make_payload(gl_multi_sample{}),
                make_payload(gl_sample_coverage{}),
                make_payload(
                    gl_blend{
                        .rgb  {.modes{.source{agl::blend_mode::src_colour}, .destination{agl::blend_mode::one_minus_src_colour}}, .algebraic_op{agl::blend_eqn_mode::subtract}},
                        .alpha{.modes{.source{agl::blend_mode::dst_alpha},  .destination{agl::blend_mode::one_minus_dst_alpha}},  .algebraic_op{agl::blend_eqn_mode::reverse_subtract}},
                        .colour{0.1f, 0.2f, 0.3f, 0.4f}
                    }
                )
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
    }
}
