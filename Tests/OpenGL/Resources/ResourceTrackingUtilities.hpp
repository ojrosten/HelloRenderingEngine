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

    template<class Resource>
    class resource_tracking_test : public resource_tracking_test_base
    {
    public:
        using resource_tracking_test_base::resource_tracking_test_base;

    protected:
        template<class Creator, class Utilizer>
            requires std::is_invocable_r_v<Resource, Creator, opengl::resourceful_context>
                  && std::is_invocable_r_v<void, Utilizer, Resource>
        void execute_tests(std::string_view expectedGPUCall, GLenum glName, Creator creator, Utilizer utilizer) {
            check_serial_tracking_non_overlapping_lifetimes(expectedGPUCall, glName, creator, utilizer);
            check_serial_tracking_overlapping_lifetimes(expectedGPUCall, glName, creator, utilizer);

            check_threaded_tracking_non_overlapping_lifetimes(expectedGPUCall, glName, creator, utilizer);
            check_threaded_tracking_overlapping_lifetimes(expectedGPUCall, glName, creator, utilizer);
        }

        ~resource_tracking_test() = default;

        resource_tracking_test(resource_tracking_test&&) noexcept = default;

        resource_tracking_test& operator=(resource_tracking_test&&) noexcept = default;
    private:
        using creator_type  = std::function<Resource(const opengl::resourceful_context&)>;
        using utilizer_type = std::function<void(const Resource&)>;

        void check_serial_tracking_non_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer);

        void check_serial_tracking_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer);

        void check_threaded_tracking_non_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer);

        void check_threaded_tracking_overlapping_lifetimes(std::string_view expectedGPUCall, GLenum glName, creator_type creator, utilizer_type utilizer);
    };
}
