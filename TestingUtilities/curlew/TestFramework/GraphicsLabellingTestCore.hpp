////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "curlew/TestFramework/GraphicsTestCore.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Debugging/Errors.hpp"
#include "avocet/OpenGL/Utilities/ObjectIdentifiers.hpp"

namespace curlew {
    template<class T>
    inline constexpr bool has_labelling_tests_v{
        requires (T& t, const window& w) { t.labelling_tests(w); }
    };

    template<test_mode Mode>
    using labelling_test_base
        = basic_graphics_test<Mode, curlew::ogl_version_and_build_selective, (Mode == test_mode::false_positive) ? curlew::platform_specific : curlew::ogl_version_and_build_specific>;

    template<test_mode Mode>
    class basic_graphics_labelling_test : public labelling_test_base<Mode>
    {
    public:
        using labelling_test_base<Mode>::labelling_test_base;

        template<class Self>
            requires has_labelling_tests_v<Self>
        void run_tests(this Self& self) {
            if constexpr(!avocet::has_ndebug()) {
                using namespace curlew;
                glfw_manager manager{};
                auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
                if(avocet::opengl::object_labels_activated()) {
                    self.labelling_tests(w);
                }
            }
        }
    protected:
        ~basic_graphics_labelling_test() = default;

        basic_graphics_labelling_test(basic_graphics_labelling_test&&)            noexcept = default;
        basic_graphics_labelling_test& operator=(basic_graphics_labelling_test&&) noexcept = default;
    };

    using graphics_labelling_test = basic_graphics_labelling_test<test_mode::standard>;
    using graphics_labelling_false_negative_test = basic_graphics_labelling_test<test_mode::false_negative>;
    using graphics_labelling_false_positive_test = basic_graphics_labelling_test<test_mode::false_positive>;
}