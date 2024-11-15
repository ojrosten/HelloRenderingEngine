////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ErrorsFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/Graphics/OpenGL/GLFunction.hpp"

#include "glad/gl.h"

namespace avocet::testing
{
    namespace agl = avocet::opengl;
    using namespace curlew;

    [[nodiscard]]
    std::filesystem::path errors_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void errors_free_test::run_tests()
    {
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        general_tests();

        if constexpr(!avocet::has_ndebug())
            debug_build_tests();
    }

    void errors_free_test::general_tests()
    {
        check_filtered_exception_thrown<std::runtime_error>(
            "Null glBindBuffer",
            [](){
                gl_breaker breaker{glBindBuffer};
                agl::gl_function{glBindBuffer}(42, 42);
            }
        );
    }

    void errors_free_test::debug_build_tests()
    {
        check_filtered_exception_thrown<std::runtime_error>(
            "Illegal call to glBindBuffer",
            [](){ agl::gl_function{glBindBuffer}(42, 42); }
        );

        check_filtered_exception_thrown<std::runtime_error>(
            "Illegal call to glBindBuffer with glBindBuffer set to nullptr, but after it's copied into gl_function",
            [](){
                agl::gl_function f{glBindBuffer};
                gl_breaker breaker{glBindBuffer};
                f(42, 42);
            }
        );
    }
}
