////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Presentation/Presentation.hpp"

#include <ranges>

namespace avocet::vulkan {
    namespace {

        [[nodiscard]]
        vk::raii::Device make_logical_device(const physical_device& physDevice, std::span<const char* const> extensions) {
            const float queuePriority{1.0}; // TO DO: allow for customization

            std::vector<std::uint32_t> uniqueFamiliesIndices{physDevice.q_family_indices.graphics.value(), physDevice.q_family_indices.present.value()};
            std::ranges::sort(uniqueFamiliesIndices);
            const auto qCreateInfos{
                std::views::transform(
                    std::ranges::subrange{uniqueFamiliesIndices.begin(), std::ranges::unique(uniqueFamiliesIndices).begin()},
                    [&queuePriority](const std::uint32_t i) -> vk::DeviceQueueCreateInfo {
                        return {
                            .sType{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO},
                            .queueFamilyIndex{i},
                            .queueCount{1},
                            .pQueuePriorities{&queuePriority}
                        };
                    }
                ) | std::ranges::to<std::vector>()
            };

            vk::PhysicalDeviceFeatures features{};

            vk::DeviceCreateInfo createInfo{
                .sType{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO},
                .queueCreateInfoCount{static_cast<std::uint32_t>(qCreateInfos.size())},
                .pQueueCreateInfos{qCreateInfos.data()},
                .enabledExtensionCount{static_cast<std::uint32_t>(extensions.size())},
                .ppEnabledExtensionNames{extensions.data()},
                .pEnabledFeatures{&features}
            };

            return physDevice.device.createDevice(createInfo);
        }


        [[nodiscard]]
        swap_chain make_swap_chain(const physical_device& physDevice, const vk::raii::Device& device, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_config& swapChainConfig, const swap_chain_support_details& swapChainDetails) {
            const auto maxImageCount{swapChainDetails.capabilities.surfaceCapabilities.maxImageCount},
                minImageCount{swapChainDetails.capabilities.surfaceCapabilities.minImageCount};

            const auto format{swapChainConfig.format_selector(swapChainDetails.formats).surfaceFormat.format};

            vk::SwapchainCreateInfoKHR createInfo{
                .surface{surfaceInfo.surface},
                .minImageCount{maxImageCount ? maxImageCount : minImageCount + 1},
                .imageFormat{format},
                .imageExtent{swapChainConfig.extent_selector(swapChainDetails.capabilities, swapChainDetails.framebuffer_extent)},
                .imageArrayLayers{1}, // Always 1 unless developing a stereoscopic app
                .imageUsage{swapChainConfig.image_usage_flags},
                .preTransform{swapChainDetails.capabilities.surfaceCapabilities.currentTransform},
                .compositeAlpha{VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR}, // Change this to blend with other windows in the windowing system
                .presentMode{swapChainConfig.present_mode_selector(swapChainDetails.present_modes)},
                .clipped{VK_FALSE},
                .oldSwapchain{VK_NULL_HANDLE}
            };

            const auto& qFamIndices{physDevice.q_family_indices};
            const std::array<uint32_t, 2> rawQFamIndices{qFamIndices.graphics.value(), qFamIndices.present.value()};
            if(qFamIndices.graphics != qFamIndices.present) {
                createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
                createInfo.queueFamilyIndexCount = 1;
                createInfo.pQueueFamilyIndices = rawQFamIndices.data();
            }
            else {
                createInfo.imageSharingMode = vk::SharingMode::eExclusive;
                createInfo.queueFamilyIndexCount = 0;
                createInfo.pQueueFamilyIndices = nullptr;
            }

            return {vk::raii::SwapchainKHR{device, createInfo}, format};
        }

        [[nodiscard]]
        std::vector<vk::ImageView> make_swap_chain_image_views(const vk::raii::Device& device, const swap_chain& swapChain, std::span<const vk::Image> images) {
            return
                std::views::transform(
                    images,
                    [&device, &swapChain](const vk::Image& image) -> vk::ImageView {
                        vk::ImageViewCreateInfo info{
                            .image{image},
                            .viewType{vk::ImageViewType::e2D},
                            .format{swapChain.format},
                            .components{
                                .r{vk::ComponentSwizzle::eIdentity},
                                .g{vk::ComponentSwizzle::eIdentity},
                                .b{vk::ComponentSwizzle::eIdentity},
                                .a{vk::ComponentSwizzle::eIdentity}
                            },
                            .subresourceRange{
                                .aspectMask{vk::ImageAspectFlagBits::eColor},
                                .baseMipLevel{0},
                                .levelCount{1},
                                .baseArrayLayer{0},
                                .layerCount{1}
                            }
                        };

                        return device.createImageView(info);
                    }
                ) | std::ranges::to<std::vector>();
        }
    }

    swap_chain_support_details::swap_chain_support_details(const vk::raii::PhysicalDevice& physDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, vk::Extent2D framebufferExtent)
        : capabilities{physDevice.getSurfaceCapabilities2KHR(surfaceInfo)}
        , formats{physDevice.getSurfaceFormats2KHR(surfaceInfo)}
        , present_modes{physDevice.getSurfacePresentModesKHR(surfaceInfo.surface)}
        , framebuffer_extent{framebufferExtent}
    {
    }

    logical_device::logical_device(const physical_device& physDevice, const device_config& deviceConfig, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo)
        : m_PhysicalDevice{physDevice.device}
        , m_Device{make_logical_device(physDevice, deviceConfig.extensions)}
        , m_GraphicsQueue{m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{physDevice.q_family_indices.graphics.value()}})}
        , m_PresentQueue{m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{physDevice.q_family_indices.present.value()}})}
        , m_SwapChain{make_swap_chain(physDevice, m_Device, surfaceInfo, deviceConfig.swap_chain, physDevice.swap_chain_details)}
        , m_SwapChainImages{m_SwapChain.chain.getImages()}
        , m_SwapChainImageViews{make_swap_chain_image_views(m_Device, m_SwapChain, m_SwapChainImages)}
    {
    }
}