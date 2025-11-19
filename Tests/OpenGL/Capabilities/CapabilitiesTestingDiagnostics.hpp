////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "avocet/OpenGL/Context/Context.hpp"

#include "sequoia/TestFramework/RegularTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class capabilities_false_negative_test final : public regular_false_negative_test
    {
    public:
        using parallelizable_type = std::false_type;

        using regular_false_negative_test::regular_false_negative_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        void test_blending(const opengl::decorated_context_base& ctx);

        void test_sample_coverage(const opengl::decorated_context_base& ctx);
    };
}