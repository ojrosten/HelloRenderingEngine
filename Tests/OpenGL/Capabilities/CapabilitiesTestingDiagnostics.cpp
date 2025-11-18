////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

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
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        test_blending(w.context());
        test_sample_coverage(w.context());
    }

    void capabilities_false_negative_test::test_blending(const opengl::decorated_context& ctx)
    {
        const gl_blend configuredBlend{
            .rgb  {.modes{.source{agl::blend_mode::src_colour}, .destination{agl::blend_mode::one_minus_src_colour}}, .algebraic_op{agl::blend_eqn_mode::subtract}},
            .alpha{.modes{.source{agl::blend_mode::dst_alpha},  .destination{agl::blend_mode::one_minus_dst_alpha}},  .algebraic_op{agl::blend_eqn_mode::reverse_subtract}},
            .colour{0.1f, 0.2f, 0.3f, 0.4f}
        };

        check(equality, "", gl_blend{}, configuredBlend);
        check(weak_equivalence, "", configuredBlend, ctx);
    }

    void capabilities_false_negative_test::test_sample_coverage(const opengl::decorated_context& ctx)
    {
        const gl_sample_coverage configuredCoverage{.coverage_val{0.5}, .invert{opengl::invert_sample_mask::yes}};

        check(equality, "", gl_sample_coverage{}, configuredCoverage);
        check(weak_equivalence, "", configuredCoverage, ctx);
    }
}