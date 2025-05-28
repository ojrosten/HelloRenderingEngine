////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "ImageTestingUtilities.hpp"

#include "sequoia/TestFramework/RegularTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class image_view_test final : public regular_test
    {
    public:
        using regular_test::regular_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    };
}
