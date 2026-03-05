////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "CapabilitiesTestingDiagnostics.hpp"
#include "CapabilitiesTestingUtilities.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing {
    using namespace opengl::capabilities;

    [[nodiscard]]
    std::filesystem::path capabilities_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void capabilities_false_negative_test::run_tests()
    {
        using namespace curlew;
        auto w{ create_window({.hiding{curlew::window_hiding_mode::on}})};

        test_blending(w.context());
        test_depth_test(w.context());
        test_sample_coverage(w.context());
        test_stencil_test(w.context());
    }

    void capabilities_false_negative_test::test_blending(const opengl::capable_context& ctx)
    {
        constexpr gl_blend configuredBlend0{
            .rgb  {.modes{.source{agl::blend_mode::dst_colour}, .destination{agl::blend_mode::one_minus_dst_colour}}, .algebraic_op{agl::blend_eqn_mode::min}},
            .alpha{.modes{.source{agl::blend_mode::src_alpha},  .destination{agl::blend_mode::one_minus_src_alpha}},  .algebraic_op{agl::blend_eqn_mode::max}},
            .colour{0.1f, 0.2f, 0.3f, 0.4f}
        };

        constexpr gl_blend configuredBlend1{
            .rgb  {.modes{.source{agl::blend_mode::src_colour}, .destination{agl::blend_mode::one_minus_src_colour}}, .algebraic_op{agl::blend_eqn_mode::subtract}},
            .alpha{.modes{.source{agl::blend_mode::dst_alpha},  .destination{agl::blend_mode::one_minus_dst_alpha}},  .algebraic_op{agl::blend_eqn_mode::reverse_subtract}},
            .colour{0.4f, 0.1f, 0.2f, 0.3f}
        };

        constexpr gl_blend configuredBlend2{
            .rgb  {.modes{.source{agl::blend_mode::const_colour}, .destination{agl::blend_mode::one_minus_const_colour}}, .algebraic_op{agl::blend_eqn_mode::add}},
            .alpha{.modes{.source{agl::blend_mode::const_alpha},  .destination{agl::blend_mode::one_minus_const_alpha}},  .algebraic_op{agl::blend_eqn_mode::max}},
            .colour{0.3f, 0.4f, 0.1f, 0.2f}
        };

        check(equality        , "", gl_blend{}      , configuredBlend0);
        check(equality        , "", configuredBlend1, configuredBlend2);
        check(weak_equivalence, "", ctx             , configuredBlend2);
    }

    void capabilities_false_negative_test::test_depth_test(const opengl::capable_context& ctx)
    {
        constexpr gl_depth_test configuredDepthTest0{
            .func{agl::comparison_mode::never},
            .mask{agl::depth_buffer_write_mode::enabled},
            .poly_offset{0.6f, 1.3f}
        };

        constexpr gl_depth_test configuredDepthTest1{
            .func{agl::comparison_mode::less},
            .mask{agl::depth_buffer_write_mode::disabled},
            .poly_offset{0.4f, 1.2f}
        };

        check(equality        , "", gl_depth_test{}, configuredDepthTest0);
        check(weak_equivalence, "", ctx            , configuredDepthTest1);
    }

    void capabilities_false_negative_test::test_sample_coverage(const opengl::capable_context& ctx)
    {
        constexpr gl_sample_coverage configuredCoverage{.coverage_val{0.5}, .invert{opengl::invert_sample_mask::yes}};

        check(equality,         "", gl_sample_coverage{}, configuredCoverage);
        check(weak_equivalence, "", ctx                 , configuredCoverage);
    }

    void capabilities_false_negative_test::test_stencil_test(const opengl::capable_context& ctx)
    {
        using enum agl::stencil_failure_mode;
        constexpr gl_stencil_test configuredStencilTest0{
            .front{
                .func{.comparison{agl::comparison_mode::less_or_equal}, .reference_value{128}, .mask{42}},
                .op{.on_failure{keep}, .on_pass_with_depth_failure{keep}, .on_pass_without_depth_failure{replace}},
                .write_mask{42}
            },
            .back{
                .func{.comparison{agl::comparison_mode::greater}, .reference_value{63}, .mask{24}},
                .op{.on_failure{replace}, .on_pass_with_depth_failure{zero}, .on_pass_without_depth_failure{decrement_wrap}},
                .write_mask{1}
            }
        };

        constexpr gl_stencil_test configuredStencilTest1{
            .front{
                .func{.comparison{agl::comparison_mode::greater_or_equal}, .reference_value{42}, .mask{128}},
                .op{.on_failure{decrement}, .on_pass_with_depth_failure{increment}, .on_pass_without_depth_failure{invert}},
                .write_mask{7}
            },
            .back{
                .func{.comparison{agl::comparison_mode::equal}, .reference_value{24}, .mask{63}},
                .op{.on_failure{zero}, .on_pass_with_depth_failure{increment_wrap}, .on_pass_without_depth_failure{decrement_wrap}},
                .write_mask{23}
            }
        };

        constexpr gl_stencil_test configuredStencilTest2{
            .front{
                .func{.comparison{agl::comparison_mode::not_equal}, .reference_value{0}, .mask{1}},
                .op{.on_failure{decrement}, .on_pass_with_depth_failure{increment}, .on_pass_without_depth_failure{invert}},
                .write_mask{27}
            },
            .back{
                .func{.comparison{agl::comparison_mode::always}, .reference_value{1}, .mask{0}},
                .op{.on_failure{zero}, .on_pass_with_depth_failure{increment_wrap}, .on_pass_without_depth_failure{decrement_wrap}},
                .write_mask{32}
            }
        };

        check(equality        , "Exposes a bug with the Intel Arc driver", gl_stencil_test{}, configuredStencilTest0);
        check(equality        , "", configuredStencilTest1, configuredStencilTest2);
        check(weak_equivalence, "", ctx                   , configuredStencilTest2);
    }

}