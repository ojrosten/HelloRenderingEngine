////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "ShaderProgramTrackingFreeTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "avocet/OpenGL/Graphics/ShaderProgram.hpp"

#include <future>
#include <latch>
#include <thread>

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path shader_program_tracking_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void shader_program_tracking_free_test::run_tests()
    {
        check_serial_tracking_non_overlapping_lifetimes();
        check_threaded_tracking_overlapping_lifetimes();
    }

    void shader_program_tracking_free_test::check_serial_tracking_non_overlapping_lifetimes()
    {

    }

    void shader_program_tracking_free_test::check_threaded_tracking_overlapping_lifetimes()
    {

    }

    void shader_program_tracking_free_test::check_program_indices(const avocet::opengl::resource_handle& prog0, const avocet::opengl::resource_handle& prog1, std::string_view tag)
    {

    }
}
