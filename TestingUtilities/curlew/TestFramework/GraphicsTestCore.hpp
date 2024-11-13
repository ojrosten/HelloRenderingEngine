////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "sequoia/TestFramework/FreeTestCore.hpp"

namespace curlew {
    using namespace sequoia::testing;

    struct exception_postprocessor {
        [[nodiscard]]
        std::string operator()(std::string message) const;
    };

    [[nodiscard]]
    std::string get_platform();

    template<test_mode Mode>
    class basic_graphics_test : public basic_test<Mode, trivial_extender>
    {
    public:
        using parallelizable_type = std::false_type;

        using basic_test<Mode, trivial_extender>::basic_test;

        [[nodiscard]]
        std::string platform() const { return get_platform(); }
    protected:
        ~basic_graphics_test() = default;

        basic_graphics_test(basic_graphics_test&&)            noexcept = default;
        basic_graphics_test& operator=(basic_graphics_test&&) noexcept = default;
    };

    /*! \anchor performance_test_alias */
    using graphics_test                = basic_graphics_test<test_mode::standard>;
    using graphics_false_negative_test = basic_graphics_test<test_mode::false_negative>;
    using graphics_false_positive_test = basic_graphics_test<test_mode::false_positive>;
}