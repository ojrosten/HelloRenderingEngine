////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "curlew/Window/RenderingSetup.hpp"

#include "sequoia/TestFramework/FreeTestCore.hpp"

namespace curlew {
    using namespace sequoia::testing;

    constexpr inline selectivity_flavour ogl_version_and_build_selective{curlew::selectivity_flavour::opengl_version | curlew::selectivity_flavour::build};
    constexpr inline selectivity_flavour os_and_renderer_selective{curlew::selectivity_flavour::os | curlew::selectivity_flavour::renderer};
    constexpr inline specificity_flavour platform_specific{specificity_flavour::os | specificity_flavour::renderer | specificity_flavour::opengl_version};
    constexpr inline specificity_flavour ogl_version_and_build_specific{curlew::specificity_flavour::opengl_version | curlew::specificity_flavour::build};
    constexpr inline specificity_flavour target_specific{platform_specific | specificity_flavour::build};
    constexpr inline specificity_flavour os_and_renderer_specific{curlew::specificity_flavour::os | curlew::specificity_flavour::renderer};

    [[nodiscard]]
    rendering_setup find_rendering_setup();

    template<test_mode Mode, selectivity_flavour Selectivity=selectivity_flavour::none, specificity_flavour Specificity=specificity_flavour::none>
    class basic_graphics_test : public basic_test<Mode, trivial_extender>
    {
    public:
        using parallelizable_type = std::false_type;
        using base_test_type = basic_test<Mode, trivial_extender>;

        using basic_test<Mode, trivial_extender>::basic_test;

        [[nodiscard]]
        std::string summary_discriminator() const
            requires (Selectivity != selectivity_flavour::none)
        {
            return rendering_setup_discriminator(find_rendering_setup(), Selectivity);
        }

        [[nodiscard]]
        std::string output_discriminator() const
            requires (Specificity != specificity_flavour::none)
        {
            return rendering_setup_discriminator(find_rendering_setup(), Specificity);
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
