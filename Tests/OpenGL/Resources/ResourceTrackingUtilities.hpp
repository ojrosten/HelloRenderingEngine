////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "curlew/TestFramework/GraphicsTestCore.hpp"

#include "avocet/OpenGL/ResourceInfrastructure/ResourceHandle.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class resource_tracking_test_base : public curlew::common_graphics_test
    {
    public:
        using curlew::common_graphics_test::common_graphics_test;

        struct resource_handles {
            using handle_t = avocet::opengl::resource_handle;

            handle_t active;
            std::optional<handle_t> after_destruction{};
        };
    protected:
        struct gpu_data {
            resource_handles resource{};
            std::vector<std::string> calls;
        };

        ~resource_tracking_test_base() = default;

        resource_tracking_test_base(resource_tracking_test_base&&) noexcept = default;

        resource_tracking_test_base& operator=(resource_tracking_test_base&&) noexcept = default;

        void check_resource_data(std::string_view tag, std::string_view expectedGPUCall, const gpu_data& data0, const gpu_data& data1);
    private:
        void check_reset_after_destruction(std::string_view tag, const resource_handles& resource);

        void check_resource_indices(std::string_view tag, const resource_handles& resource0, const resource_handles& resource1);
    };

    class resource_tracking_test : public resource_tracking_test_base
    {
    public:
        using resource_tracking_test_base::resource_tracking_test_base;

    protected:
        void execute_tests() {
            check_serial_tracking_non_overlapping_lifetimes();
            check_serial_tracking_overlapping_lifetimes();

            check_threaded_tracking_non_overlapping_lifetimes();
            check_threaded_tracking_overlapping_lifetimes();
        }

        ~resource_tracking_test() = default;

        resource_tracking_test(resource_tracking_test&&) noexcept = default;

        resource_tracking_test& operator=(resource_tracking_test&&) noexcept = default;
    private:

        void check_serial_tracking_non_overlapping_lifetimes();

        void check_serial_tracking_overlapping_lifetimes();

        void check_threaded_tracking_non_overlapping_lifetimes();

        void check_threaded_tracking_overlapping_lifetimes();
    };
}
