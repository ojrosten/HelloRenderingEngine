////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "InfiniteLoopFreeTest.hpp"
#include "avocet/Debugging/OpenGL/Errors.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path infinite_loop_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void infinite_loop_free_test::run_tests()
    {
        if(is_windows()) {
            using namespace curlew;
            glfw_manager manager{};
            if(is_intel(manager.find_rendering_setup().renderer)) {
                check_filtered_exception_thrown<std::runtime_error>(
                    "Repeatedly calling glGetError",
                    []() { opengl::check_for_basic_errors(std::source_location::current()); }
                );
            }
        }
    }
}
