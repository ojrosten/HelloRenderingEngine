////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "curlew/TestFramework/GraphicsTestCore.hpp"

#include "avocet/OpenGL/Resources/ResourceHandle.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class shader_program_tracking_free_test final : public curlew::common_graphics_test
    {
    public:
        using curlew::common_graphics_test::common_graphics_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        struct gpu_data {
            avocet::opengl::resource_handle prog{};
            std::vector<std::string> calls;
        };

        void check_serial_tracking_non_overlapping_lifetimes();

        void check_serial_tracking_overlapping_lifetimes();

        void check_parallel_tracking_non_overlapping_lifetimes();

        void check_parallel_tracking_overlapping_lifetimes();

        void check_program_indices(std::string_view tag, const gpu_data& data0, const gpu_data& data1);
    };
}
