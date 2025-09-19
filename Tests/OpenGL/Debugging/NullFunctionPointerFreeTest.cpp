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
            "Invoking gl_function such that it delegates to a null function pointer",
            [](){ return agl::gl_function{agl::unchecked_debug_output, &GladGLContext::GetError}(agl::decorated_context{}); }
        );

        check_exception_thrown<std::runtime_error>(
            "Check for basic errors with no context",
            [](){
                agl::check_for_basic_errors(agl::decorated_context{}, {.fn_name{}, .loc{std::source_location::current()}}, agl::num_messages{10});
            }
        );
    }
}
