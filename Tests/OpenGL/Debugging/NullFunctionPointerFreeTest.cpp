////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "NullFunctionPointerFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"

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
        using namespace curlew;

        using decorator_type = std::function<void(const agl::context&, const agl::decorator_data)>;

        check_exception_thrown<std::runtime_error>(
            "Invoking gl_function such that it delegates to a null function pointer",
            []() {
                agl::decorated_context nothingLoaded{agl::debugging_mode::off, [](GladGLContext ctx) { return ctx; }, decorator_type{}, decorator_type{}};
                return agl::gl_function{&GladGLContext::GetError}(nothingLoaded);
            }
        );
    }
}
