////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ViewportTest.hpp"

namespace avocet::testing
{
    namespace {
        [[nodiscard]]
        constexpr bool good_extent(const discrete_extent& extent) noexcept {
            return (extent.width > 0) && (extent.height > 0);
        }

        [[nodiscard]]
        std::optional<viewport> refit(const discrete_extent& nominalExtent, const discrete_extent& availableExtent) {
            return null_viewport;
        }
    }

    using opt_viewport = std::optional<avocet::viewport>;

    [[nodiscard]]
    std::filesystem::path viewport_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void viewport_test::run_tests()
    {
        avocet::viewport x{{}, {800, 600}}, y{{42, 1729}, {5, 7}};
        check_semantics("", x, y);

        test_refit();
    }

    void viewport_test::test_refit() {
    }
}
