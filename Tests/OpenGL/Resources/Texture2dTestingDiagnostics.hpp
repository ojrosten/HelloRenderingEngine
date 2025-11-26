////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "Texture2dTestingUtilities.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class texture_2d_false_negative_test final : public curlew::common_graphics_move_only_false_negative_test
    {
    public:
        using curlew::common_graphics_move_only_false_negative_test::common_graphics_move_only_false_negative_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    };
}
