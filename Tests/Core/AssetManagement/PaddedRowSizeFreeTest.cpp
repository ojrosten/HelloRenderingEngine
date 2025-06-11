////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "PaddedRowSizeFreeTest.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

namespace avocet::testing
{
    namespace {
        [[nodiscard]]
        std::size_t padded_row_size(std::size_t width, colour_channels channels, std::size_t bytesPerChannel, alignment rowAlignment) {
            constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};
            if(!bytesPerChannel)
                throw std::runtime_error{"padded_row_size: bytes per channel must be > 0"};

            if(maxVal / bytesPerChannel < channels.raw_value())
                throw std::runtime_error{std::format("padded_row_size: channels ({}) * bytes per channel ({}) exceeds max allowed value, {}", channels, bytesPerChannel, maxVal)};

            const auto bytesPerTexel{channels.raw_value() * bytesPerChannel};
            if(bytesPerTexel && (maxVal / bytesPerTexel < width))
                throw std::runtime_error{std::format("padded_row_size: width ({}) * bytes per texel ({}) exceeds max allowed value, {}", width, bytesPerTexel, maxVal)};

            const auto nominalWidth{width * bytesPerTexel};
            const auto unpaddedBytes{nominalWidth % rowAlignment.raw_value()};
            if(!unpaddedBytes)
                return nominalWidth;

            if(maxVal - rowAlignment.raw_value() < nominalWidth - unpaddedBytes)
                throw std::runtime_error{std::format("padded_row_size: nominal row size ({}) aligned to ({}) bytes will be padded to exceed max allowed value, {}", nominalWidth, rowAlignment, maxVal)};

            return nominalWidth - unpaddedBytes + rowAlignment.raw_value();
        }
    }

    [[nodiscard]]
    std::filesystem::path padded_row_size_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void padded_row_size_free_test::run_tests()
    {
        constexpr auto maxVal{std::numeric_limits<std::size_t>::max()};

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return testing::padded_row_size(1, colour_channels{1}, 0, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return testing::padded_row_size(1 + maxVal / 2, colour_channels{2}, 1, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return testing::padded_row_size(1, colour_channels{2}, 1 + maxVal / 2, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return testing::padded_row_size(2, colour_channels{1 + maxVal / 2}, 1, alignment{1}); }
        );

        check_exception_thrown<std::runtime_error>(
            "",
            []() { return testing::padded_row_size(maxVal, colour_channels{1}, 1, alignment{2}); }
        );

        check(equality, "", testing::padded_row_size(0, colour_channels{1}, 1, alignment{1}), 0uz);
        check(equality, "", testing::padded_row_size(1, colour_channels{0}, 1, alignment{1}), 0uz);
        check(equality, "", testing::padded_row_size(1, colour_channels{1}, 1, alignment{1}), 1uz);
        check(equality, "", testing::padded_row_size(2, colour_channels{1}, 1, alignment{1}), 2uz);
        check(equality, "", testing::padded_row_size(1, colour_channels{2}, 1, alignment{1}), 2uz);
        check(equality, "", testing::padded_row_size(1, colour_channels{1}, 2, alignment{1}), 2uz);
        check(equality, "", testing::padded_row_size(2, colour_channels{3}, 4, alignment{1}), 24uz);

        check(equality, "", testing::padded_row_size(1, colour_channels{1}, 1, alignment{2}), 2uz);
        check(equality, "", testing::padded_row_size(1, colour_channels{1}, 1, alignment{4}), 4uz);
        check(equality, "", testing::padded_row_size(3, colour_channels{3}, 1, alignment{2}), 10uz);
        check(equality, "", testing::padded_row_size(2, colour_channels{3}, 2, alignment{8}), 16uz);
        check(equality, "", testing::padded_row_size(2, colour_channels{3}, 4, alignment{8}), 24uz);

        check(equality, "", testing::padded_row_size(maxVal, colour_channels{1}, 1, alignment{1}), maxVal);
    }
}
