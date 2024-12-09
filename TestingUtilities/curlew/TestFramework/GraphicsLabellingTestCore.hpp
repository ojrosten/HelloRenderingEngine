////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "curlew/TestFramework/GraphicsTestCore.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/Debugging/OpenGL/Errors.hpp"

namespace curlew {
    [[nodiscard]]
    std::string get_object_label(avocet::opengl::object_identifier identifier, const avocet::opengl::resource_handle& handle, std::size_t expectedSize);

    template<class T>
    inline constexpr bool has_labelling_tests_v{
        requires (T & t) { t.labelling_tests(); }
    };

    template<test_mode Mode>
    class basic_graphics_labelling_test : public basic_graphics_test<Mode, curlew::ogl_version_and_build_selective, curlew::specificity_flavour::none>
    {
    public:
        using base_graphics_test_type = basic_graphics_test<Mode, curlew::ogl_version_and_build_selective, curlew::specificity_flavour::none>;
        using base_graphics_test_type::base_graphics_test_type;

        bool check_object_label(const reporter& description, avocet::opengl::object_identifier identifier, const avocet::opengl::resource_handle& handle, std::string_view expected) {
            return this->check(equivalence, description, get_object_label(identifier, handle, expected.size()), expected);
        }

        template<class Self>
            requires has_labelling_tests_v<Self>
        void run_tests(this Self& self) {
            if constexpr(!avocet::has_ndebug()) {
                using namespace curlew;
                glfw_manager manager{};
                auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
                if(avocet::opengl::object_labels_activated()) {
                    self.labelling_tests();
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