////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "PaddedRowSizeFreeTest.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

#include <limits>

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path padded_row_size_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void padded_row_size_free_test::run_tests()
    {
        constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};

        check_exception_thrown<std::out_of_range>(
            "Zero bytes per channel",
            [](){ return padded_row_size(1, colour_channels{1}, 0, alignment{1}); }
        );

        check_exception_thrown<std::out_of_range>(
            "",
            [](){ return padded_row_size(1, colour_channels{2}, 1+maxVal/2, alignment{1}); }
        );

        check_exception_thrown<std::out_of_range>(
            "",
            [](){ return padded_row_size(1 + maxVal / 2, colour_channels{2}, 1, alignment{1}); }
        );

        check_exception_thrown<std::out_of_range>(
            "",
            [](){ return padded_row_size(maxVal, colour_channels{1}, 1, alignment{2}); }
        );

        check(equality, "", padded_row_size(0, colour_channels{1}, 1, alignment{1}), 0uz);
        check(equality, "", padded_row_size(1, colour_channels{0}, 1, alignment{1}), 0uz);
        check(equality, "", padded_row_size(1, colour_channels{1}, 1, alignment{1}), 1uz);
        check(equality, "", padded_row_size(2, colour_channels{1}, 1, alignment{1}), 2uz);
        check(equality, "", padded_row_size(1, colour_channels{2}, 1, alignment{1}), 2uz);
        check(equality, "", padded_row_size(1, colour_channels{1}, 2, alignment{1}), 2uz);
        check(equality, "", padded_row_size(2, colour_channels{3}, 4, alignment{1}), 24uz);

        check(equality, "", padded_row_size(1, colour_channels{1}, 1, alignment{2}), 2uz);
        check(equality, "", padded_row_size(1, colour_channels{1}, 1, alignment{4}), 4uz);
        check(equality, "", padded_row_size(2, colour_channels{3}, 1, alignment{4}), 8uz);
        check(equality, "", padded_row_size(2, colour_channels{3}, 2, alignment{8}), 16uz);
    }
}
