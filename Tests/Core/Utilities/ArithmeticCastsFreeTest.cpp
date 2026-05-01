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

    void arithmetic_casts_free_test::run_tests()
    {
        STATIC_CHECK(safe_integral_conversion_v<int, int>);
        STATIC_CHECK(safe_integral_conversion_v<std::size_t, std::size_t>);
        STATIC_CHECK(safe_integral_conversion_v<std::uint32_t, std::int64_t>);

        STATIC_CHECK(!safe_integral_conversion_v<int, std::size_t>);
        STATIC_CHECK(!safe_integral_conversion_v<std::int32_t, std::uint32_t>);
        STATIC_CHECK(!safe_integral_conversion_v<std::int32_t, std::uint64_t>);

        check_exception_thrown<std::domain_error>(
            "",
            []() { return convert_value_to<std::size_t>(-1); }
        );

        check_exception_thrown<std::domain_error>(
            "",
            []() { return convert_value_to<std::int32_t>(std::numeric_limits<std::int64_t>::max()); }
        );

        check_exception_thrown<std::domain_error>(
            "",
            []() { return convert_value_to<std::int32_t>(std::numeric_limits<std::int64_t>::lowest()); }
        );

        check_exception_thrown<std::domain_error>(
            "",
            []() { return convert_value_to<std::uint32_t>(std::numeric_limits<std::uint64_t>::max()); }
        );

        check_exception_thrown<std::domain_error>(
            "",
            []() { return convert_value_to<std::int32_t>(std::numeric_limits<std::uint32_t>::max()); }
        );

        check(equality, "", convert_value_to<std::uint32_t>(std::numeric_limits<std::uint64_t>::max()    >> 32), std::numeric_limits<std::uint32_t>::max());
        check(equality, "", convert_value_to<std::int32_t >(std::numeric_limits<std::int64_t >::max()    >> 32), std::numeric_limits<std::int32_t >::max());
        check(equality, "", convert_value_to<std::int32_t >(std::numeric_limits<std::int64_t >::lowest() >> 32), std::numeric_limits<std::int32_t >::lowest());
        check(equality, "", convert_value_to<std::int32_t >(std::numeric_limits<std::uint32_t>::max()     /  2), std::numeric_limits<std::int32_t >::max());

        check(equality, "", convert_value_to<std::int32_t >(std::int64_t{}), std::int32_t{});
    }
}
