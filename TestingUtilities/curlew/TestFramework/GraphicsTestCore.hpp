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

    template<class Fn>
    class [[nodiscard]] gl_breaker{
        Fn* m_pFn{};
        Fn m_Fn;
    public:
        gl_breaker(Fn& fn) : m_pFn{&fn}, m_Fn{std::exchange(fn, nullptr)} {}

        ~gl_breaker() { *m_pFn = m_Fn; }
    };

    [[nodiscard]]
    std::string get_platform();

    template<test_mode Mode>
    class basic_graphics_test : public basic_test<Mode, trivial_extender>
    {
    public:
        using parallelizable_type = std::false_type;

        using basic_test<Mode, trivial_extender>::basic_test;

        template<class E, class Fn>
        bool check_filtered_exception_thrown(const reporter& description, Fn&& function)
        {
            return basic_test<Mode, trivial_extender>::check_exception_thrown<E>(description, std::forward<Fn>(function), exception_postprocessor{});
        }
    protected:
        ~basic_graphics_test() = default;

        basic_graphics_test(basic_graphics_test&&)            noexcept = default;
        basic_graphics_test& operator=(basic_graphics_test&&) noexcept = default;
    };

    using graphics_test                = basic_graphics_test<test_mode::standard>;
    using graphics_false_negative_test = basic_graphics_test<test_mode::false_negative>;
    using graphics_false_positive_test = basic_graphics_test<test_mode::false_positive>;

    template<test_mode Mode>
    class basic_target_dependent_graphics_test : public basic_graphics_test<Mode>
    {
    public:
        using basic_graphics_test<Mode>::basic_graphics_test;

        [[nodiscard]]
        std::string output_discriminator() const { return get_platform(); }
    protected:
        ~basic_target_dependent_graphics_test() = default;

        basic_target_dependent_graphics_test(basic_target_dependent_graphics_test&&)            noexcept = default;
        basic_target_dependent_graphics_test& operator=(basic_target_dependent_graphics_test&&) noexcept = default;
    };

    using target_dependent_graphics_test                = basic_target_dependent_graphics_test<test_mode::standard>;
    using target_dependent_graphics_false_negative_test = basic_target_dependent_graphics_test<test_mode::false_negative>;
    using target_dependent_graphics_false_positive_test = basic_target_dependent_graphics_test<test_mode::false_positive>;
}