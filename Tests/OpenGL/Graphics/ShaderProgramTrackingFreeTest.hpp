////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/FreeTestCore.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Graphics/ResourceHandle.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class shader_program_tracking_free_test final : public free_test
    {
    public:
        using parallelizable_type = std::false_type;

        using free_test::free_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        void check_serial_tracking_non_overlapping_lifetimes(curlew::glfw_manager& manager);

        void check_threaded_tracking_overlapping_lifetimes(curlew::glfw_manager& manager);

        void check_program_indices(std::string_view tag, const avocet::opengl::resource_handle& prog0, const avocet::opengl::resource_handle& prog1);
    };
}
