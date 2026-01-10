////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Vulkan/Setup/Devices.hpp"

#include <filesystem>

namespace avocet::vulkan {
    struct frames_in_flight {
        std::uint32_t value{};

        friend constexpr auto operator<=>(const frames_in_flight&, const frames_in_flight&) noexcept = default;
    };

    struct swap_chain {
        vk::raii::SwapchainKHR chain;
        vk::Format             format;
    };

    struct swap_chain_configuration {
        std::function<vk::SurfaceFormat2KHR(std::span<const vk::SurfaceFormat2KHR>)>  format_selector{};
        std::function<vk::PresentModeKHR(std::span<const vk::PresentModeKHR>)>        present_mode_selector{};
        std::function<vk::Extent2D(const vk::SurfaceCapabilities2KHR&, vk::Extent2D)> extent_selector{};

        vk::ImageUsageFlags image_usage_flags{};
    };

    struct swap_chain_support_details {
        swap_chain_support_details(const vk::raii::PhysicalDevice& physDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo);

        vk::SurfaceCapabilities2KHR        capabilities{};
        std::vector<vk::SurfaceFormat2KHR> formats{};
        std::vector<vk::PresentModeKHR>    present_modes{};
    };

    class swap_chain_plus_images {
        swap_chain_configuration         m_Config;
        swap_chain                       m_Chain;
        std::vector<vk::Image>           m_Images;
        std::vector<vk::raii::ImageView> m_ImageViews;

        // Mac doesn't like two non-trivial swap chains in existence at the same time
        // However, it's useful for implementing rebuild, so it's made private
        swap_chain_plus_images& operator=(swap_chain_plus_images&&) noexcept = default;
    public:
        swap_chain_plus_images(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_configuration& swapChainConfig, const swap_chain_support_details& swapChainDetails, vk::Extent2D extent);

        swap_chain_plus_images(swap_chain_plus_images&&) noexcept = default;

        swap_chain_plus_images& rebuild(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_configuration& swapChainConfig, const swap_chain_support_details& swapChainDetails, vk::Extent2D extent);

        [[nodiscard]]
        const swap_chain_configuration& config() const noexcept { return m_Config; }

        [[nodiscard]]
        const vk::raii::SwapchainKHR& chain() const noexcept { return m_Chain.chain; }

        [[nodiscard]]
        vk::Format format() const noexcept { return m_Chain.format; }

        [[nodiscard]]
        std::span<const vk::raii::ImageView> image_views() const noexcept { return m_ImageViews; }
    };

    struct presentation_configuration {
        std::size_t width{800}, height{600};
        std::string name;
        device_configuration device_config{};
        swap_chain_configuration swap_chain_config{};
        frames_in_flight max_frames_in_flight{2};
    };


    class presentable {
        vk::raii::SurfaceKHR   m_Surface;
        logical_device         m_LogicalDevice;
        swap_chain_plus_images m_SwapChain;

        [[nodiscard]]
        swap_chain_support_details extract_swap_chain_support() const;

        [[nodiscard]]
        vk::PhysicalDeviceSurfaceInfo2KHR get_surface_info() const { return {.surface{m_Surface}}; }
    public:
        presentable(const instance& vkInstance, const presentation_configuration& presentationConfig, std::function<vk::raii::SurfaceKHR(const instance&)> surfaceCreator, vk::Extent2D extent);

        void rebuild_swapchain(vk::Extent2D extent);

        [[nodiscard]]
        const logical_device& get_logical_device() const noexcept { return m_LogicalDevice; }

        [[nodiscard]]
        const swap_chain_plus_images& get_swap_chain() const noexcept { return m_SwapChain; }

        void wait_idle() const;
    };
}
