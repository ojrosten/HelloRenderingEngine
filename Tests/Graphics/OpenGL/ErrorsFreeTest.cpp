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
    namespace {
        template<class Fn>
        class [[nodiscard]] gl_breaker{
            Fn* m_pFn{};
            Fn m_Fn;
        public:
            gl_breaker(Fn& fn) : m_pFn{&fn}, m_Fn{ std::exchange(fn, nullptr) } {}

            ~gl_breaker() { *m_pFn = m_Fn; }
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

        check_exception_thrown<std::runtime_error>(
            "Null glGetError",
            [](){
                gl_breaker breaker{glGetError};
                agl::check_for_basic_errors(std::source_location::current());
            }
        );

        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        check_exception_thrown<std::runtime_error>(
            "Null glBindBuffer",
            [](){
                gl_breaker breaker{glBindBuffer};
                agl::gl_function{glBindBuffer}(42, 42);
            }
        );

        check_exception_thrown<std::runtime_error>(
            "Illegal call to glBindBuffer",
            [](){ agl::gl_function{glBindBuffer}(42, 42);}
        );
    }
}
