////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Vulkan/Setup/Instance.hpp"

#include <functional>
#include <optional>

namespace avocet::vulkan {
    struct queue_family_indices {
        std::optional<std::uint32_t> graphics{},
                                     present{};
    };

    struct physical_device {
        vk::raii::PhysicalDevice device;
        queue_family_indices     q_family_indices{};
    };

    struct device_configuration {
        std::function<physical_device(std::span<const vk::raii::PhysicalDevice>, std::span<const std::string>, const vk::PhysicalDeviceSurfaceInfo2KHR&)> selector{};
        std::vector<std::string> extensions{};
    };

    class logical_device {
        physical_device  m_PhysicalDevice;
        vk::raii::Device m_Device;
        vk::raii::Queue  m_GraphicsQueue,
                         m_PresentQueue; // TO DO: make these optional?
    public:
        logical_device(physical_device physDevice, const device_configuration& deviceConfig);

        [[nodiscard]]
        const vk::raii::Device& device() const noexcept { return m_Device; }

        [[nodiscard]]
        const queue_family_indices& q_family_indices() const noexcept { return m_PhysicalDevice.q_family_indices; }

        [[nodiscard]]
        const vk::raii::Queue& get_graphics_queue() const noexcept { return m_GraphicsQueue; }

        const physical_device& get_physical_device() const noexcept { return m_PhysicalDevice; }
    };

}