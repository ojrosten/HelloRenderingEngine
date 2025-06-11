////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "PaddedImageSizeFreeTest.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

namespace avocet::testing
{
    namespace {
        constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};
    }

    [[nodiscard]]
    std::filesystem::path padded_image_size_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void padded_image_size_free_test::run_tests()
    {
        test_padded_row_size();
        test_safe_image_size();
    }

    void padded_image_size_free_test::test_padded_row_size()
    {
        check_exception_thrown<std::runtime_error>(
            "",
            []() { return padded_row_size(1, colour_channels{1}, 0, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "Zero bytes per channel considered a bug, even if the image holds no data",
            []() { return padded_row_size(0, colour_channels{1}, 0, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return padded_row_size(1 + maxVal / 2, colour_channels{2}, 1, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return padded_row_size(1, colour_channels{2}, 1 + maxVal / 2, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return padded_row_size(2, colour_channels{1 + maxVal / 2}, 1, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return padded_row_size(maxVal, colour_channels{1}, 1, alignment{2}); }
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
        check(equality, "", padded_row_size(3, colour_channels{3}, 1, alignment{2}), 10uz);
        check(equality, "", padded_row_size(2, colour_channels{3}, 2, alignment{8}), 16uz);
        check(equality, "", padded_row_size(2, colour_channels{3}, 4, alignment{8}), 24uz);

        check(equality, "", padded_row_size(maxVal, colour_channels{1}, 1, alignment{1}), maxVal);
    }

    void padded_image_size_free_test::test_safe_image_size()
    {
        check_exception_thrown<std::runtime_error>(
            "",
            []() { return safe_image_size(1 + maxVal / 2, 2); }
        );

        check(equality, "", safe_image_size(1, 1), 1uz);
        check(equality, "", safe_image_size(1, 2), 2uz);
        check(equality, "", safe_image_size(3, 4), 12uz);

        check(equality, "", safe_image_size(1, maxVal), maxVal);
        check(equality, "", safe_image_size(maxVal, 1), maxVal);
    }
}
