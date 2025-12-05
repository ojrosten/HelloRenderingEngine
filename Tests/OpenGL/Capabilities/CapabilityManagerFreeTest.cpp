////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CapabilityManagerFreeTest.hpp"
#include "CapabilitiesTestingUtilities.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Utilities/Casts.hpp"
#include "avocet/OpenGL/Capabilities/CapabilitiesConfiguration.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"
#include "sequoia/TestFramework/StateTransitionUtilities.hpp"

#include <format>

namespace {
    namespace agl = avocet::opengl;

    using payload_type = agl::capable_context::payload_type;

    template<class... Caps>
    [[nodiscard]]
    payload_type make_payload(const Caps&... caps) {
        payload_type payload{};
        ((std::get<std::optional<Caps>>(payload) = caps), ...);
        return payload;
    }

    template<class... Caps>
    [[nodiscard]]
    payload_type set_payload(const agl::capable_context& ctx, const payload_type& hostPayload, const Caps&... targetCaps) {
        ctx.new_payload(hostPayload);
        const auto payload{make_payload(targetCaps...)};
        ctx.new_payload(payload);
        return payload;
    }

    using namespace agl::capabilities;

    constexpr gl_blend configuredBlend{
        .rgb  {.modes{.source{agl::blend_mode::src_colour}, .destination{agl::blend_mode::one_minus_src_colour}}, .algebraic_op{agl::blend_eqn_mode::subtract}},
        .alpha{.modes{.source{agl::blend_mode::dst_alpha},  .destination{agl::blend_mode::one_minus_dst_alpha}},  .algebraic_op{agl::blend_eqn_mode::reverse_subtract}},
        .colour{0.1f, 0.2f, 0.3f, 0.4f}
    };

    using enum agl::stencil_failure_mode;
    constexpr gl_stencil_test configuredStencilTest{
        .front{
            .func{.comparison{agl::comparison_mode::greater}, .reference_value{42}, .mask{128}},
            .op{.on_failure{decrement}, .on_pass_with_depth_failure{increment}, .on_pass_without_depth_failure{invert}},
            .write_mask{7}
        },
        .back{
            .func{.comparison{agl::comparison_mode::less}, .reference_value{24}, .mask{63}},
            .op{.on_failure{zero}, .on_pass_with_depth_failure{increment_wrap}, .on_pass_without_depth_failure{decrement_wrap}},
            .write_mask{23}
        }
    };

    constexpr gl_depth_test configuredDepthTest{
        .func{agl::comparison_mode::greater_or_equal},
        .mask{agl::depth_buffer_write_mode::disabled},
        .poly_offset{0.4f, 1.2f}
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
        auto w{create_window({.hiding{curlew::window_hiding_mode::on}})};
        const auto& ctx{w.context()};

        namespace agl = avocet::opengl;
        using namespace agl::capabilities;

        enum node_name {
            none                     = 0,
            blend                    = 1,
            depth_test               = 2,
            multi_sample             = 3,
            sample_alpha_to_coverage = 4,
            sample_coverage          = 5,
            stencil_test             = 6,
            configured_blend         = 7,
            configured_depth_test    = 8,
            configured_stencil_test  = 9
        };

        using graph_type = transition_checker<payload_type>::transition_graph;

        graph_type graph{
            {
                { // Begin Edges: none
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    },
                    {
                        node_name::blend,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_blend{}); }
                    },
                    {
                        node_name::depth_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_depth_test{}); }
                    },
                    {
                        node_name::multi_sample,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_multi_sample{}); }
                    },

                    {
                        node_name::sample_alpha_to_coverage,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_sample_alpha_to_coverage{}); }
                    },
                    {
                        node_name::sample_coverage,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_sample_coverage{}); }
                    },
                    {
                        node_name::stencil_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_stencil_test{}); }
                    },
                    {
                        node_name::configured_blend,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, configuredBlend); }
                    },
                    {
                        node_name::configured_depth_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, configuredDepthTest); }
                    },
                    {
                        node_name::configured_stencil_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, configuredStencilTest); }
                    },
                }, // End   Edges: none
                {  // Begin Edges: blend
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    },
                    {
                        node_name::blend,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_blend{}); }
                    },
                    {
                        node_name::multi_sample,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_multi_sample{}); }
                    },
                    {
                        node_name::configured_blend,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, configuredBlend); }
                    }
                }, // End   Edges: blend
                {  // Begin Edges: depth_test
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    },
                    {
                        node_name::depth_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_depth_test{}); }
                    },
                    {
                        node_name::configured_depth_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, configuredDepthTest); }
                    }
                }, // End    Edges: depth_test
                {  // Begin Edges: multi_sample
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    }
                }, // End   Edges: multi_sample
                {  // Begin Edges: sample_alpha_to_coverage
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    }
                }, // End   Edges: sample_alpha_to_coverage
                {  // Begin Edges: sample_coverage
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    }
                }, // End   Edges: sample_coverage
                {  // Begin Edges: stencil_test
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    },
                    {
                        node_name::configured_stencil_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, configuredStencilTest); }
                    },
                }, // End   Edges: stencil_test
                {  // Begin Edges: configured_blend
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    },
                    {
                        node_name::blend,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_blend{}); }
                    },
                }, // End   Edges: configured_blend
                {  // Begin Edges: configured_depth_test
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    },
                    {
                        node_name::depth_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_depth_test{}); }
                    },
                }, // End   Edges: configured_depth_test
                {  // Begin Edges: configured_stencil_test
                    {
                        node_name::none,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload); }
                    },
                    {
                        node_name::stencil_test,
                        report(""),
                        [&ctx](const payload_type& hostPayload) { return set_payload(ctx, hostPayload, gl_stencil_test{}); }
                    },
                }, // End   Edges: configured_stencil_test
            },
            {
                payload_type{},
                make_payload(gl_blend{}),
                make_payload(gl_depth_test{}),
                make_payload(gl_multi_sample{}),
                make_payload(gl_sample_alpha_to_coverage{}),
                make_payload(gl_sample_coverage{}),
                make_payload(gl_stencil_test{}),
                make_payload(configuredBlend),
                make_payload(configuredDepthTest),
                make_payload(configuredStencilTest)
            }
        };

        auto checker{
            [&ctx, this](std::string_view description, const payload_type& obtained, const payload_type& predicted) {
                check(equality, description, obtained, predicted);
                check(weak_equivalence, description, ctx, predicted);
            }
        };

        transition_checker<payload_type>::check("", graph, checker);
    }
}
