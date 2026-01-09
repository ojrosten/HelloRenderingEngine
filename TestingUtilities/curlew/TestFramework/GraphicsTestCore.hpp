////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"

namespace curlew {
    using namespace sequoia::testing;

    constexpr inline selectivity_flavour os_and_renderer_selective{curlew::selectivity_flavour::os | curlew::selectivity_flavour::renderer};
    constexpr inline specificity_flavour platform_specific{specificity_flavour::os | specificity_flavour::renderer | specificity_flavour::opengl_version};
    constexpr inline specificity_flavour os_and_renderer_specific{curlew::specificity_flavour::os | curlew::specificity_flavour::renderer};

    class test_window_manager {
        inline static curlew::glfw_manager st_Manager{avocet::vulkan::instance_info{}};
    public:
        [[nodiscard]]
        static opengl_rendering_setup find_rendering_setup();

        [[nodiscard]]
        static curlew::opengl_window create_window(const curlew::opengl_window_config& config) {
            return st_Manager.create_window(config);
        }
    };

    template<test_mode Mode, selectivity_flavour Selectivity=selectivity_flavour::none, specificity_flavour Specificity=specificity_flavour::none, class Extender=trivial_extender>
    class basic_graphics_test : public basic_test<Mode, Extender>
    {
    public:
        using parallelizable_type = std::false_type;
        using base_test_type = basic_test<Mode, Extender>;

        using basic_test<Mode, Extender>::basic_test;

        [[nodiscard]]
        std::string summary_discriminator() const
            requires (Selectivity != selectivity_flavour::none)
        {
            return rendering_setup_discriminator(test_window_manager::find_rendering_setup(), Selectivity);
        }

        [[nodiscard]]
        std::string output_discriminator() const
            requires (Specificity != specificity_flavour::none)
        {
            return rendering_setup_discriminator(test_window_manager::find_rendering_setup(), Specificity);
        }
    protected:
        ~basic_graphics_test() = default;

        basic_graphics_test(basic_graphics_test&&)            noexcept = default;
        basic_graphics_test& operator=(basic_graphics_test&&) noexcept = default;

        [[nodiscard]]
        curlew::opengl_window create_window(const curlew::opengl_window_config& config) { return test_window_manager::create_window(config); }
    };

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_test = basic_graphics_test<test_mode::standard, Selectivity, Specificity>;

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_false_negative_test = basic_graphics_test<test_mode::false_negative, Selectivity, Specificity>;

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_false_positive_test = basic_graphics_test<test_mode::false_positive, Selectivity, Specificity>;

    using common_graphics_test                =                graphics_test<selectivity_flavour::none, specificity_flavour::none>;
    using common_graphics_false_negative_test = graphics_false_negative_test<selectivity_flavour::none, specificity_flavour::none>;
    using common_graphics_false_positive_test = graphics_false_positive_test<selectivity_flavour::none, specificity_flavour::none>;


    template<test_mode Mode, selectivity_flavour Selectivity, specificity_flavour Specificity>
    using basic_graphics_move_only_test = basic_graphics_test<Mode, Selectivity, Specificity, move_only_extender<Mode>>;

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_move_only_test = basic_graphics_move_only_test<test_mode::standard, Selectivity, Specificity>;

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_move_only_false_negative_test = basic_graphics_move_only_test<test_mode::false_negative, Selectivity, Specificity>;

    template<selectivity_flavour Selectivity, specificity_flavour Specificity>
    using graphics_move_only_false_positive_test = basic_graphics_move_only_test<test_mode::false_positive, Selectivity, Specificity>;

    using common_graphics_move_only_test                =                graphics_move_only_test<selectivity_flavour::none, specificity_flavour::none>;
    using common_graphics_move_only_false_negative_test = graphics_move_only_false_negative_test<selectivity_flavour::none, specificity_flavour::none>;
    using common_graphics_move_only_false_positive_test = graphics_move_only_false_positive_test<selectivity_flavour::none, specificity_flavour::none>;
}
