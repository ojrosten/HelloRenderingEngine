////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "ImageTestingUtilities.hpp"

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class unique_image_test final : public move_only_test
    {
    public:
        using move_only_test::move_only_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();

        void check_semantics_via_image_data(const reporter& description, const image_data& first, const image_data& second) {
            move_only_test::check_semantics(
                description,
                to_unique_image(first),
                to_unique_image(second),
                first,
                second,
                image_data{},
                image_data{}
            );
        }
    };
}
