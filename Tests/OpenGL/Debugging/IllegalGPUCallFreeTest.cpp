////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "IllegalGPUCallFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"

#include "glad/gl.h"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path illegal_gpu_call_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void illegal_gpu_call_free_test::run_tests()
    {
        if constexpr(!avocet::has_ndebug())
            debug_build_tests();
    }

    void illegal_gpu_call_free_test::debug_build_tests()
    {
        using namespace curlew;

        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
        const auto& ctx{w.context()};

        check_exception_thrown<std::runtime_error>(
            "Illegal call to glBindBuffer (void return)",
            [&ctx](){ agl::gl_function{&GladGLContext::BindBuffer}(ctx, 42, 42); }
        );

        check_exception_thrown<std::runtime_error>(
            "Illegal call to glCreateShader (non-void return)",
            [&ctx]() { return agl::gl_function{&GladGLContext::CreateShader}(ctx, GL_ARRAY_BUFFER); }
        );
    }
}
