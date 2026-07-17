////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "FormattingFreeTest.hpp"
#include "avocet/Core/Formatting/Formatting.hpp"

namespace avocet::testing
{
    namespace
    {
        enum class whatever {foo, bar};
    }

    [[nodiscard]]
    std::filesystem::path formatting_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void formatting_free_test::run_tests()
    {
        check_exception_thrown<std::runtime_error>(
            "Put error_message output under version control through the auto-generated exceptions file",
            []() { throw std::runtime_error{avocet::error_message("whatever", whatever::foo)}; }
        );
    }
}
