////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ErrorsFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/Graphics/OpenGL/Core.hpp"

namespace avocet::testing
{
    namespace {
        class [[nodiscard]] gl_breaker{
            using fn_ptr = GLenum(*)();
            fn_ptr m_Fn;
        public:
            gl_breaker() : m_Fn{std::exchange(glGetError, nullptr)} {}

            ~gl_breaker() { glGetError = m_Fn; }
        };
    }

    [[nodiscard]]
    std::filesystem::path errors_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void errors_free_test::run_tests()
    {
        namespace agl = avocet::opengl;

        check_exception_thrown<std::runtime_error>("Null opengl pointer", [](){ gl_breaker breaker{}; agl::check_for_errors(std::source_location::current()); });

        curlew::glfw_manager manager{};
        auto w{manager.create_window()};

        check_exception_thrown<std::runtime_error>("", [](){ agl::gl_function_invoker{glBindBuffer, 42, 42}; });
    }
}
