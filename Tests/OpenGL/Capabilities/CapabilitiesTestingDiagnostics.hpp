////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"

#include "curlew/TestFramework/GraphicsTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class capabilities_false_negative_test final : public curlew::graphics_false_negative_test<curlew::selectivity_flavour::none, curlew::specificity_flavour::renderer>
    {
    public:
        using curlew::graphics_false_negative_test<curlew::selectivity_flavour::none, curlew::specificity_flavour::renderer>::graphics_false_negative_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        void test_blending(const opengl::decorated_context& ctx);

        void test_sample_coverage(const opengl::decorated_context& ctx);

        void test_stencil_test(const opengl::decorated_context& ctx);
    };
}