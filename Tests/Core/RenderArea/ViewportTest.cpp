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
        check(equality, "", refit({0, 1}, {1, 1}), null_viewport);
        check(equality, "", refit({1, 0}, {1, 1}), null_viewport);
        check(equality, "", refit({1, 1}, {0, 1}), null_viewport);
        check(equality, "", refit({1, 1}, {1, 0}), null_viewport);

        check(equality, "Unable to exactly centralize", refit({1, 1}, {1, 2}), opt_viewport{{{}, {1, 1}}});
        check(equality, "", refit({1, 1}, {1, 3}), opt_viewport{{{0, 1}, {1, 1}}});
        check(equality, "Computed height of zero", refit({2, 1}, {1, 1}), null_viewport);
        check(equality, "", refit({1, 1}, {2, 3}), opt_viewport{{{0, 0}, {2, 2}}});
        check(equality, "", refit({2, 2}, {1, 3}), opt_viewport{{{0, 1}, {1, 1}}});
        check(equality, "Unable to respect aspect ratio", refit({5, 2}, {4, 3}), opt_viewport{{{0, 1}, {4, 1}}});

        check(equality, "Unable to exactly centralize", refit({1, 1}, {2, 1}), opt_viewport{{{}, {1, 1}}});
        check(equality, "", refit({1, 1}, {3, 1}), opt_viewport{{{1, 0}, {1, 1}}});
        check(equality, "Computed width of zero", refit({1, 2}, {1, 1}), null_viewport);
        check(equality, "", refit({1, 1}, {3, 2}), opt_viewport{{{0, 0}, {2, 2}}});
        check(equality, "", refit({2, 2}, {3, 1}), opt_viewport{{{1, 0}, {1, 1}}});
        check(equality, "Unable to respect aspect ratio", refit({2, 5}, {3, 4}), opt_viewport{{{1, 0}, {1, 4}}});

        check(equality, "", refit({1, 1}, {1, 1}), opt_viewport{{{0, 0}, {1, 1}}});
        check(equality, "", refit({1, 1}, {2, 2}), opt_viewport{{{0, 0}, {2, 2}}});
        check(equality, "", refit({2, 2}, {1, 1}), opt_viewport{{{0, 0}, {1, 1}}});
    }
}
