////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "avocet/OpenGL/Graphics/ResourceHandle.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include "sequoia/TestFramework/FreeTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class shader_program_threading_free_test final : public free_test
    {
    public:
        using free_test::free_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        void check_serial_tracking(curlew::glfw_manager& manager);

        void check_serial_tracking_overlapping_lifetimes(curlew::glfw_manager& manager);

        void check_threaded_tracking(curlew::glfw_manager& manager);

        void check_program_indices(const avocet::opengl::resource_handle& prog0, const avocet::opengl::resource_handle& prog1, std::string_view tag);
    };
}
