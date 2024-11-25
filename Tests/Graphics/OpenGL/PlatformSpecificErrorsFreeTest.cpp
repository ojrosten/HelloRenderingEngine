////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "PlatformSpecificErrorsFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/Graphics/OpenGL/GLFunction.hpp"

#include "glad/gl.h"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path platform_specific_errors_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void platform_specific_errors_free_test::run_tests()
    {
        namespace agl = avocet::opengl;
        using namespace curlew;

        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        check_filtered_exception_thrown<std::runtime_error>(
            "Null glBindBuffer",
            [](){
                gl_breaker breaker{glBindBuffer};
                agl::gl_function{glBindBuffer}(42, 42);
            }
        );
    }
}
