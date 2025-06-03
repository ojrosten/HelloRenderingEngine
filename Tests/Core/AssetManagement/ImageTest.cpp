////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ImageTest.hpp"
namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path image_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void image_test::run_tests()
    {
        test_padded_row_size();
        test_image();
    }

    void image_test::test_padded_row_size() {
        check(equality, "", padded_row_size(0, colour_channels{1}, alignment{1}, 1uz), 0uz);
        check(equality, "", padded_row_size(1, colour_channels{1}, alignment{1}, 1uz), 1uz);
        check(equality, "", padded_row_size(1, colour_channels{1}, alignment{2}, 1uz), 2uz);
        check(equality, "", padded_row_size(2, colour_channels{1}, alignment{1}, 1uz), 2uz);
        check(equality, "", padded_row_size(2, colour_channels{1}, alignment{2}, 1uz), 2uz);
        check(equality, "", padded_row_size(2, colour_channels{1}, alignment{4}, 1uz), 4uz);

        check(equality, "", padded_row_size(1, colour_channels{2}, alignment{1}, 1uz), 2uz);
        check(equality, "", padded_row_size(1, colour_channels{2}, alignment{2}, 1uz), 2uz);
        check(equality, "", padded_row_size(1, colour_channels{2}, alignment{4}, 1uz), 4uz);
        check(equality, "", padded_row_size(2, colour_channels{2}, alignment{2}, 1uz), 4uz);
        check(equality, "", padded_row_size(2, colour_channels{2}, alignment{4}, 1uz), 4uz);

        check(equality, "", padded_row_size(3, colour_channels{2}, alignment{4}, 1uz), 8uz);
    }

    void image_test::test_image() {
        execute_image_tests(*this, std::identity{}, image_data{}, image_data{});
    }
}
