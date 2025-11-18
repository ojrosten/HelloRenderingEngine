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

namespace avocet::testing {
    using namespace opengl::capabilities;

    [[nodiscard]]
    std::filesystem::path capabilities_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void capabilities_false_negative_test::run_tests()
    {
        test_blending();
        test_sample_coverage();
    }

    void capabilities_false_negative_test::test_blending()
    {
        check(
            equality,
            "",
            gl_blend{},
            gl_blend{
                .rgb  {.modes{.source{agl::blend_mode::src_colour}, .destination{agl::blend_mode::one_minus_src_colour}}, .algebraic_op{agl::blend_eqn_mode::subtract}},
                .alpha{.modes{.source{agl::blend_mode::dst_alpha},  .destination{agl::blend_mode::one_minus_dst_alpha}},  .algebraic_op{agl::blend_eqn_mode::reverse_subtract}},
                .colour{0.1f, 0.2f, 0.3f, 0.4f}
            }
        );
    }

    void capabilities_false_negative_test::test_sample_coverage()
    {
        check(
            equality,
            "",
            gl_sample_coverage{},
            gl_sample_coverage{.coverage_val{0.5}, .invert{opengl::invert_sample_mask::yes}}
        );
    }
}