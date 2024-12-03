////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "sequoia/TestFramework/FreeTestCore.hpp"
#include "sequoia/Core/Logic/Bitmask.hpp"

namespace curlew {
    enum class selectivity_flavour : uint64_t { none = 0, os = 1, renderer = 2, opengl_version = 4, build = 8 };
    enum class specificity_flavour : uint64_t { none = 0, os = 1, renderer = 2, opengl_version = 4, build = 8 };
}

inline namespace sequoia_bitmask {
    template<>
    struct as_bitmask<curlew::selectivity_flavour> : std::true_type {};

    template<>
    struct as_bitmask<curlew::specificity_flavour> : std::true_type {};
}

namespace curlew {
    using namespace sequoia::testing;

    constexpr inline selectivity_flavour ogl_version_and_build_selective{curlew::selectivity_flavour::opengl_version | curlew::selectivity_flavour::build};
    constexpr inline specificity_flavour platform_specific{specificity_flavour::os | specificity_flavour::renderer | specificity_flavour::opengl_version};
    constexpr inline specificity_flavour target_specific{platform_specific | specificity_flavour::build};

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
    std::string make_discriminator(selectivity_flavour selectivity);

    [[nodiscard]]
    std::string make_discriminator(specificity_flavour specificity);

    template<test_mode Mode, selectivity_flavour Selectivity=selectivity_flavour::none, specificity_flavour Specificity=specificity_flavour::none>
    class basic_graphics_test : public basic_test<Mode, trivial_extender>
    {
    public:
        using parallelizable_type = std::false_type;

        using basic_test<Mode, trivial_extender>::basic_test;

        template<class E, class Fn>
        bool check_filtered_exception_thrown(const reporter& description, Fn&& function)
        {
            return basic_test<Mode, trivial_extender>::template check_exception_thrown<E>(description, std::forward<Fn>(function), exception_postprocessor{});
        }

        [[nodiscard]]
        std::string summary_discriminator() const
            requires (Selectivity != selectivity_flavour::none)
        {
            return make_discriminator(Selectivity);
        }

        [[nodiscard]]
        std::string output_discriminator() const
            requires (Specificity != specificity_flavour::none)
        {
            return make_discriminator(Specificity);
        }
    protected:
        ~basic_graphics_test() = default;

        basic_graphics_test(basic_graphics_test&&)            noexcept = default;
        basic_graphics_test& operator=(basic_graphics_test&&) noexcept = default;
    };

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_test = basic_graphics_test<test_mode::standard, Selectivity, Specificity>;

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_false_negative_test = basic_graphics_test<test_mode::false_negative, Selectivity, Specificity>;

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_false_positive_test = basic_graphics_test<test_mode::false_positive, Selectivity, Specificity>;

    using common_graphics_test = graphics_test<selectivity_flavour::none, specificity_flavour::none>;
}
