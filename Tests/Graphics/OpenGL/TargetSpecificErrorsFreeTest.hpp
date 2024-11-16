////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "curlew/TestFramework/GraphicsTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class target_specific_errors_free_test final : public curlew::target_selective_graphics_test
    {
    public:
        using curlew::target_selective_graphics_test::target_selective_graphics_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();

        void debug_build_tests();
    };
}
