////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Presentation/Presentation.hpp"
#include "avocet/Vulkan/Common/Casts.hpp"
#include "avocet/Vulkan/Common/Conversions.hpp"

#include <ranges>

namespace avocet::vulkan {
    namespace {

        [[nodiscard]]
        swap_chain make_swap_chain(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_configuration& swapChainConfig, const swap_chain_support_details& swapChainDetails, vk::Extent2D extent) {
            const auto requestedMinImageCount{
                [&swapChainDetails]() {
                    const auto minImageCount{swapChainDetails.capabilities.surfaceCapabilities.minImageCount},
                               maxImageCount{swapChainDetails.capabilities.surfaceCapabilities.maxImageCount};

                    return (minImageCount == maxImageCount) ? minImageCount : minImageCount + 1;
                }()
            };

            const auto format{swapChainConfig.format_selector(swapChainDetails.formats).surfaceFormat.format};

            vk::SwapchainCreateInfoKHR createInfo{
                .surface{surfaceInfo.surface},
                .minImageCount{requestedMinImageCount},
                .imageFormat{format},
                .imageExtent{swapChainConfig.extent_selector(swapChainDetails.capabilities, extent)},
                .imageArrayLayers{1}, // Always 1 unless developing a stereoscopic app
                .imageUsage{swapChainConfig.image_usage_flags},
                .preTransform{swapChainDetails.capabilities.surfaceCapabilities.currentTransform},
                .compositeAlpha{VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR}, // Change this to blend with other windows in the windowing system
                .presentMode{swapChainConfig.present_mode_selector(swapChainDetails.present_modes)},
                .clipped{VK_FALSE},
                .oldSwapchain{VK_NULL_HANDLE}
            };

            const auto& qFamIndices{logicalDevice.q_family_indices()};
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

            return {vk::raii::SwapchainKHR{logicalDevice.device(), createInfo}, format};
        }

        [[nodiscard]]
        std::vector<vk::raii::ImageView> make_swap_chain_image_views(const vk::raii::Device& device, const vk::Format& format, std::span<const vk::Image> images) {
            return
                std::views::transform(
                    images,
                    [&device, &format](const vk::Image& image) -> vk::raii::ImageView {
                        vk::ImageViewCreateInfo info{
                            .image{image},
                            .viewType{vk::ImageViewType::e2D},
                            .format{format},
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

    swap_chain_support_details::swap_chain_support_details(const vk::raii::PhysicalDevice& physDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo)
        : capabilities{physDevice.getSurfaceCapabilities2KHR(surfaceInfo)}
        , formats{physDevice.getSurfaceFormats2KHR(surfaceInfo)}
        , present_modes{physDevice.getSurfacePresentModesKHR(surfaceInfo.surface)}
    {
    }

    swap_chain_plus_images::swap_chain_plus_images(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_configuration& swapChainConfig, const swap_chain_support_details& swapChainDetails, vk::Extent2D extent)
        : m_Config{swapChainConfig}
        , m_Chain{make_swap_chain(logicalDevice, surfaceInfo, m_Config, swapChainDetails, extent)}
        , m_Images{m_Chain.chain.getImages()}
        , m_ImageViews{make_swap_chain_image_views(logicalDevice.device(), m_Chain.format, m_Images)}
    {
    }

    swap_chain_plus_images& swap_chain_plus_images::rebuild(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_configuration& swapChainConfig, const swap_chain_support_details& swapChainDetails, vk::Extent2D extent) {
        m_Chain.chain = {nullptr};
        return *this = swap_chain_plus_images{logicalDevice, surfaceInfo, swapChainConfig, swapChainDetails, extent};
    }

    presentable::presentable(const instance& vkInstance, const presentation_configuration& presentationConfig, std::function<vk::raii::SurfaceKHR(const instance&)> surfaceCreator, vk::Extent2D extent)
        : m_Surface{surfaceCreator(vkInstance)}
        , m_LogicalDevice{
            presentationConfig.device_config.selector(vkInstance.get().enumeratePhysicalDevices(), presentationConfig.device_config.extensions, get_surface_info()),
            presentationConfig.device_config
          }
        , m_SwapChain{m_LogicalDevice, get_surface_info(), presentationConfig.swap_chain_config, extract_swap_chain_support(), extent}
    {
    }

    [[nodiscard]]
    swap_chain_support_details presentable::extract_swap_chain_support() const {
        return {m_LogicalDevice.get_physical_device().device, get_surface_info()};
    }

    void presentable::rebuild_swapchain(vk::Extent2D extent) {
        wait_idle();
        m_SwapChain.rebuild(m_LogicalDevice, get_surface_info(), m_SwapChain.config(), extract_swap_chain_support(), extent);
    }

    void presentable::wait_idle() const { m_LogicalDevice.device().waitIdle(); }
}
