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
        test_sample_coverage(w.context());
        test_stencil_test(w.context());
    }

    void capabilities_false_negative_test::test_blending(const opengl::capable_context& ctx)
    {
        constexpr gl_blend configuredBlend{
            .rgb  {.modes{.source{agl::blend_mode::src_colour}, .destination{agl::blend_mode::one_minus_src_colour}}, .algebraic_op{agl::blend_eqn_mode::subtract}},
            .alpha{.modes{.source{agl::blend_mode::dst_alpha},  .destination{agl::blend_mode::one_minus_dst_alpha}},  .algebraic_op{agl::blend_eqn_mode::reverse_subtract}},
            .colour{0.1f, 0.2f, 0.3f, 0.4f}
        };

        check(equality        , "", gl_blend{}, configuredBlend);
        check(weak_equivalence, "", ctx       , configuredBlend);
    }

    void capabilities_false_negative_test::test_depth_test(const opengl::capable_context& ctx)
    {
        constexpr gl_depth_test configuredDepthTest{
            .func{agl::comparison_mode::greater_or_equal},
            .mask{agl::depth_buffer_write_mode::disabled},
            .poly_offset{0.4f, 1.2f}
        };

        check(equality        , "", gl_depth_test{}, configuredDepthTest);
        check(weak_equivalence, "", ctx            , configuredDepthTest);
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

        check(equality        , "", gl_stencil_test{}, configuredStencilTest);
        check(weak_equivalence, "", ctx              , configuredStencilTest);
    }

}