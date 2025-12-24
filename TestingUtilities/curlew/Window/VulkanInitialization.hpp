////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Vulkan/Presentation/Presentation.hpp"

namespace curlew::vulkan {
    [[nodiscard]]
    std::vector<const char*> build_vulkan_extensions(std::span<const char* const> requestedExtensions);

    struct device_selector {
        [[nodiscard]]
        avocet::vulkan::physical_device operator()(std::span<const vk::raii::PhysicalDevice> devices, std::span<const char* const> requiredExtensions, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, vk::Extent2D framebufferExtent) const;
    };

    struct swap_chain_format_selector {
        [[nodiscard]]
        vk::SurfaceFormat2KHR operator()(std::span<const vk::SurfaceFormat2KHR> available) const;
    };

    struct swap_chain_present_mode_selector {
        [[nodiscard]]
        vk::PresentModeKHR operator()(std::span<const vk::PresentModeKHR> available) const;
    };

    struct swap_chain_extent_selector {
        [[nodiscard]]
        vk::Extent2D operator()(const vk::SurfaceCapabilities2KHR& capabilities, vk::Extent2D framebufferExtent) const;
    };
}