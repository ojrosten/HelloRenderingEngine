////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ArithmeticCastsFreeTest.hpp"
#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path arithmetic_casts_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    template<std::integral To, std::integral From>
    inline constexpr bool has_lossless_conversion_v{
        sequoia::initializable_from<To, From>
    };

    // If val is in the the range of the return type perform a
    // static_cast; else throw a std::domain_error
    template<std::integral To, std::integral From>
    [[nodiscard]]
    To checked_conversion_to(From val) noexcept(has_lossless_conversion_v<To, From>)
    {
        if constexpr(not has_lossless_conversion_v<To, From>) {
            if(constexpr auto maxVal{std::numeric_limits<To>::max()}; val > maxVal)
                throw std::domain_error{std::format("Value {} exceeds max value {} of target type", val, maxVal)};

            if constexpr(std::is_signed_v<From>) {
                if constexpr(std::is_signed_v<To>) {
                    if(constexpr auto lowestVal{std::numeric_limits<To>::lowest()}; val < lowestVal)
                        throw std::domain_error{std::format("Value {} lower than lowest value {} of target type", val, lowestVal)};
                }
                else if(val < 0)
                    throw std::domain_error{std::format("Value {} lower than lowest value {} of target type", val, 0)};
            }
        }

        return static_cast<To>(val);
    }

    void arithmetic_casts_free_test::run_tests()
    {
        check_exception_thrown<std::domain_error>(
            "",
            []() {
                return checked_conversion_to<std::int32_t>(std::numeric_limits<std::int64_t>::max());
            }
        );

        check_exception_thrown<std::domain_error>(
            "",
            []() {
                return checked_conversion_to<std::int32_t>(std::numeric_limits<std::uint32_t>::max());
            }
        );

        check_exception_thrown<std::domain_error>(
            "",
            []() {
                return checked_conversion_to<std::uint32_t>(std::int32_t{-1});
            }
        );

        check(equality, "", checked_conversion_to<int>(42), 42);

        check(equality, "", checked_conversion_to<std:: int32_t>(std::numeric_limits<std::int64_t >::   max() >> 32), std::numeric_limits<std::int32_t>::max());
        check(equality, "", checked_conversion_to<std:: int32_t>(std::numeric_limits<std::int64_t >::lowest() >> 32), std::numeric_limits<std::int32_t>::lowest());
        check(equality, "", checked_conversion_to<std::uint32_t>(std::numeric_limits<std::uint64_t>::   max() >> 32), std::numeric_limits<std::uint32_t>::max());
        check(equality, "", checked_conversion_to<std:: int32_t>(std::numeric_limits<std::uint32_t>::   max()  /  2), std::numeric_limits<std::int32_t>::max());
    }
}
