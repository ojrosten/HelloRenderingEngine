////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "MultipleIllegalGPUCallsFreeTest.hpp"
#include "avocet/OpenGL/Debugging/Errors.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include "glad/gl.h"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path multiple_illegal_gpu_calls_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void multiple_illegal_gpu_calls_free_test::run_tests()
    {
        if constexpr(!avocet::has_ndebug())
            debug_build_tests();
    }

    void multiple_illegal_gpu_calls_free_test::debug_build_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};
        const auto& ctx{w.context()};

        namespace agl = avocet::opengl;
        if(agl::debug_output_supported(ctx)) {
            check_exception_thrown<std::runtime_error>(
                "At least two errors",
                [&ctx](){
                    ctx.glad_context().BindBuffer(GL_ARRAY_BUFFER, 42);
                    ctx.glad_context().CreateShader(0);
                    agl::check_for_advanced_errors(ctx, {.fn_name{"Multiple fns"}, .loc{std::source_location::current()}}, agl::num_messages{10});
                }
            );
        }
    }
}
