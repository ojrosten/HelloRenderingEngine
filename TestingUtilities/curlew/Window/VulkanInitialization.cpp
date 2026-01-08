////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/VulkanInitialization.hpp"

#include "avocet/Vulkan/Common/Formatting.hpp"

#include <ranges>
#include <print>

#include "GLFW/glfw3.h"

namespace {
    [[nodiscard]]
    bool has_required_extensions(const vk::raii::PhysicalDevice& device, std::span<const char* const> requiredExtensions) {
        const auto extensions{device.enumerateDeviceExtensionProperties()};
        for(const auto& required : requiredExtensions) {
            if(std::ranges::find_if(extensions, [required](std::string_view actualName) { return std::string_view{required} == actualName; }, [](const vk::ExtensionProperties& prop) { return prop.extensionName.data(); }) == extensions.end())
                return false;
        }

        return true;
    }
}

namespace curlew::vulkan {
    [[nodiscard]]
    std::vector<const char*> build_vulkan_extensions(std::span<const char* const> requestedExtensions) {
        std::uint32_t count{};
        const char** names{glfwGetRequiredInstanceExtensions(&count)};

        std::vector<const char*> extensions{std::from_range, std::span<const char*>(names, count)};
        for(const auto requestedExt : requestedExtensions) {
            auto found{std::ranges::find(extensions, requestedExt)};
            if(found == extensions.end())
                extensions.push_back(requestedExt);
        }

        return extensions;
    }

    [[nodiscard]]
    avocet::vulkan::physical_device device_selector::operator()(std::span<const vk::raii::PhysicalDevice> devices, std::span<const char* const> requiredExtensions, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo) const {
        // For now, print out details of all discovered devices
        for(const auto& d : devices) {
            std::println("--Device--\n{}\n-Features-\n{}\n--------", d.getProperties2().properties, d.getFeatures2().features);
        }

        for(const auto& device : devices) {
            if(!has_required_extensions(device, requiredExtensions))
                continue;

            const avocet::vulkan::swap_chain_support_details swapChainDetails{device, surfaceInfo};
            if(swapChainDetails.formats.empty() || swapChainDetails.present_modes.empty())
                continue;

            avocet::vulkan::queue_family_indices qFamilyIndices{};

            const auto qFamiliesProperties{device.getQueueFamilyProperties2()};
            for(auto i : std::views::iota(std::uint32_t{}, static_cast<std::uint32_t>(qFamiliesProperties.size()))) {
                const vk::QueueFamilyProperties2& properties{qFamiliesProperties[i]};
                if((properties.queueFamilyProperties.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)
                    qFamilyIndices.graphics = i;

                if(device.getSurfaceSupportKHR(i, surfaceInfo.surface))
                    qFamilyIndices.present = i;

                if(qFamilyIndices.graphics && qFamilyIndices.present)
                    return {device, qFamilyIndices, swapChainDetails};
            }
        }

        throw std::runtime_error{"No devices found by Vulkan which support the graphics and presentation surface"};
    };

    [[nodiscard]]
    vk::SurfaceFormat2KHR swap_chain_format_selector::operator()(std::span<const vk::SurfaceFormat2KHR> available) const {
        if(available.empty())
            throw std::runtime_error{"No available surface formats"};

        auto found{
            std::ranges::find_if(
                available,
                [](const vk::SurfaceFormat2KHR& format) {
                    return (format.surfaceFormat.format == vk::Format::eB8G8R8Srgb) && (format.surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear);
                }
            )
        };
        if(found != available.end())
            return *found;

        return available.front();
    }

    [[nodiscard]]
    vk::PresentModeKHR swap_chain_present_mode_selector::operator()(std::span<const vk::PresentModeKHR> available) const {
        auto found{std::ranges::find(available, vk::PresentModeKHR::eMailbox)};
        if(found != available.end())
            return *found;

        return vk::PresentModeKHR::eFifo;
    }

    [[nodiscard]]
    vk::Extent2D swap_chain_extent_selector::operator()(const vk::SurfaceCapabilities2KHR& capabilities, vk::Extent2D framebufferExtent) const {
        constexpr auto maxSize{std::numeric_limits<uint32_t>::max()};
        const auto& surfCaps{capabilities.surfaceCapabilities};
        if(surfCaps.currentExtent.width != maxSize) {
            return surfCaps.currentExtent;
        }
        else {
            return {
                .width {std::clamp(framebufferExtent.width,  surfCaps.minImageExtent.width,  surfCaps.maxImageExtent.width)},
                .height{std::clamp(framebufferExtent.height, surfCaps.minImageExtent.height, surfCaps.maxImageExtent.height)}
            };
        }
    }
}