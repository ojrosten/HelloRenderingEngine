////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "NullFunctionPointerFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/ContextResolver.hpp"

#include "glad/gl.h"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path null_function_pointer_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void null_function_pointer_free_test::run_tests()
    {
        namespace agl = avocet::opengl;
        using namespace curlew;

        check_exception_thrown<std::runtime_error>(
            "Constructing gl_function with a null pointer",
            [](){
                auto glGetErrorPtr = glGetError;
                gl_breaker breaker{glGetErrorPtr};
                return agl::gl_function{agl::unchecked_debug_output, glGetError}();
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Null glGetError when checking for basic errors",
            [](){
                auto glGetErrorPtr = glGetError;
                gl_breaker breaker{glGetErrorPtr};
                agl::check_for_basic_errors(agl::num_messages{10}, std::source_location::current());
            }
        );

        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        check_exception_thrown<std::runtime_error>(
            "Null glBindBuffer",
            [](){
                auto glBindBufferPtr = glBindBuffer;
                gl_breaker breaker{glBindBufferPtr};
                agl::gl_function{glBindBuffer}(42, 42);
            }
        );
    }
}
