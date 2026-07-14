////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"

#include "avocet/OpenGL/Context/DecoratedContext.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"

#include <iostream>

namespace curlew {
    using namespace sequoia::testing;

    constexpr inline selectivity_flavour os_and_renderer_selective{curlew::selectivity_flavour::os | curlew::selectivity_flavour::renderer};
    constexpr inline specificity_flavour platform_specific{specificity_flavour::os | specificity_flavour::renderer | specificity_flavour::opengl_version};
    constexpr inline specificity_flavour os_and_renderer_specific{curlew::specificity_flavour::os | curlew::specificity_flavour::renderer};

    class test_window_manager {
        inline static curlew::glfw_manager st_Manager{
            []() -> curlew::glfw_manager {
                try {
                    return {};
                }
                catch(const std::exception& e) {
                    std::println(std::cerr, "Failure during static initialization of GLFW: {}", e.what());
                    std::terminate();
                }
            }()
        };
    public:
        [[nodiscard]]
        static rendering_setup find_rendering_setup();

        [[nodiscard]]
        static curlew::window create_window(const curlew::window_config& config) {
            return st_Manager.create_window(config);
        }

        static void detach_current_context() {
            st_Manager.detach_current_context();
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
        curlew::window_config make_default_config(avocet::discrete_extent extent) const {
            return {
                .extent{extent},
                .name{""},
                .hiding{curlew::window_hiding_mode::on},
                .debug_mode{agl::debugging_mode::dynamic},
                .prologue{},
                .epilogue{
                     agl::standard_error_checker{
                         agl::num_messages{10},
                         agl::default_debug_info_processor{{}, curlew::ignored_warnings(test_window_manager::find_rendering_setup())}
                     }
                 },
                .compensate{agl::attempt_to_compensate_for_driver_bugs::yes},
                .samples{1}
            };
        }

        [[nodiscard]]
        curlew::window create_window(const curlew::window_config& config) const { return test_window_manager::create_window(config); }

        [[nodiscard]]
        curlew::window create_default_window(avocet::discrete_extent extent) const {
            return create_window(make_default_config(extent));
        }

        void check_gpu_cleanup(const avocet::opengl::decorated_context& ctx,
                               avocet::opengl::glad_ctx_ptr_to_mem_fn_ptr_type<GLboolean, GLuint> ptrToGLFn,
                               const avocet::opengl::resource_handle& handle,
                               std::source_location loc=std::source_location::current())
        {
            namespace agl = avocet::opengl;
            if (this->check(this->report({"Index needs to be non-null for next check to be meaningful", loc}), handle != agl::resource_handle{})) {
                this->check(this->report({"Destruction has cleaned up the resource", loc}), not agl::gl_function{ptrToGLFn}(ctx, handle.index()));
            }
        }
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
