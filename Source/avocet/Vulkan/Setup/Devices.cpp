////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Setup/Devices.hpp"

#include "avocet/Vulkan/Common/Casts.hpp"
#include "avocet/Vulkan/Common/Conversions.hpp"

#include <ranges>

namespace avocet::vulkan {
    namespace {

        [[nodiscard]]
        vk::raii::Device make_logical_device(const physical_device& physDevice, std::span<const std::string> extensions) {
            const float queuePriority{1.0}; // TO DO: allow for customization

            std::vector<std::uint32_t> uniqueFamiliesIndices{physDevice.q_family_indices.graphics.value(), physDevice.q_family_indices.present.value()};
            std::ranges::sort(uniqueFamiliesIndices);
            const auto qCreateInfos{
                std::views::transform(
                    std::ranges::subrange{uniqueFamiliesIndices.begin(), std::ranges::unique(uniqueFamiliesIndices).begin()},
                    [&queuePriority](const std::uint32_t i) -> vk::DeviceQueueCreateInfo {
                        return {
                            .queueFamilyIndex{i},
                            .queueCount{1},
                            .pQueuePriorities{&queuePriority}
                        };
                    }
                ) | std::ranges::to<std::vector>()
            };

            vk::PhysicalDeviceFeatures features{};

            vk::PhysicalDeviceSynchronization2Features syncFeatures{
                .synchronization2{true}
            };

            auto rawExtensions{to_cstyle_strings(extensions)};
            vk::DeviceCreateInfo createInfo{
                .pNext{&syncFeatures},
                .queueCreateInfoCount{to_uint32(qCreateInfos.size())},
                .pQueueCreateInfos{qCreateInfos.data()},
                .enabledExtensionCount{to_uint32(rawExtensions.size())},
                .ppEnabledExtensionNames{rawExtensions.data()},
                .pEnabledFeatures{&features}
            };

            return physDevice.device.createDevice(createInfo);
        }
    }

    logical_device::logical_device(physical_device physDevice, const device_configuration& deviceConfig)
        : m_PhysicalDevice{std::move(physDevice)}
        , m_Device{make_logical_device(m_PhysicalDevice, deviceConfig.extensions)}
        , m_GraphicsQueue{m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{m_PhysicalDevice.q_family_indices.graphics.value()}})}
        , m_PresentQueue {m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{m_PhysicalDevice.q_family_indices.present.value()}})}
    {
    }
}