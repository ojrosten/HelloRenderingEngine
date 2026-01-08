////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Vulkan/Common/VulkanConfig.hpp"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <span>
#include <string>

namespace avocet::vulkan {
    struct product_info {
        std::string   name{};
        std::uint32_t version{VK_MAKE_VERSION(1, 0, 0)};
    };

    struct application_info {
        product_info app{}, engine{};
    };

    struct queue_family_indices {
        std::optional<std::uint32_t>
            graphics{},
            present{};
    };

    struct instance_info {
        vk::InstanceCreateFlags flags{vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR};
        application_info app_info{};
    };

    struct swap_chain_support_details {
        swap_chain_support_details(const vk::raii::PhysicalDevice& physDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, vk::Extent2D framebufferExtent);

        vk::SurfaceCapabilities2KHR        capabilities{};
        std::vector<vk::SurfaceFormat2KHR> formats{};
        std::vector<vk::PresentModeKHR>    present_modes{};
        vk::Extent2D                       framebuffer_extent{};
    };

    struct physical_device {
        vk::raii::PhysicalDevice   device;
        queue_family_indices       q_family_indices{};
        swap_chain_support_details swap_chain_details;
    };

    struct swap_chain_config {
        std::function<vk::SurfaceFormat2KHR (std::span<const vk::SurfaceFormat2KHR>)> format_selector{};
        std::function<vk::PresentModeKHR (std::span<const vk::PresentModeKHR>)> present_mode_selector{};
        std::function<vk::Extent2D (const vk::SurfaceCapabilities2KHR&, vk::Extent2D)> extent_selector{};

        vk::ImageUsageFlags image_usage_flags{};
    };

    struct device_config {
        std::function<physical_device(std::span<const vk::raii::PhysicalDevice>, std::span<const char* const>, const vk::PhysicalDeviceSurfaceInfo2KHR&, vk::Extent2D)> selector{};
        std::vector<const char*> extensions{};
        swap_chain_config swap_chain{};
    };

    class logical_device {
        physical_device  m_PhysicalDevice;
        vk::raii::Device m_Device;
        vk::raii::Queue  m_GraphicsQueue,
                         m_PresentQueue; // TO DO: make these optional?
    public:
        logical_device(physical_device physDevice, const device_config& deviceConfig);

        [[nodiscard]]
        const vk::raii::Device& device() const noexcept { return m_Device; }

        [[nodiscard]]
        const queue_family_indices& q_family_indices() const noexcept { return m_PhysicalDevice.q_family_indices; }

        [[nodiscard]]
        const vk::raii::Queue& get_graphics_queue() const noexcept { return m_GraphicsQueue; }

        [[nodiscard]]
        const swap_chain_support_details& swap_chain_support() const noexcept { return m_PhysicalDevice.swap_chain_details; }
    };

    struct swap_chain_and_format {
        vk::raii::SwapchainKHR chain;
        vk::Format             format;
    };

    class swap_chain {
        swap_chain_and_format            m_Chain;
        std::vector<vk::Image>           m_Images;
        std::vector<vk::raii::ImageView> m_ImageViews;
    public:
        swap_chain(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_config& swapChainConfig, const swap_chain_support_details& swapChainDetails);

        [[nodiscard]]
        std::span<const vk::raii::ImageView> image_views() const noexcept { return m_ImageViews; }

        [[nodiscard]]
        vk::Format format() const noexcept { return m_Chain.format; }

        [[nodiscard]]
        const vk::raii::SwapchainKHR& chain() const noexcept { return m_Chain.chain; }
    };

    struct presentation_config {
        std::size_t width{800}, height{600};
        instance_info create_info{};
        std::vector<const char*> validation_layers{};
        std::vector<const char*> extensions{};
        device_config device_config{};
        std::uint32_t max_frames_in_flight{2};
    };

    class command_buffer {
        vk::raii::CommandBuffer m_CommandBuffer;

        vk::raii::Semaphore m_ImageAvailable,
                            m_RenderFinished;

        void record_cmd_buffer(const vk::raii::RenderPass& renderPass, const vk::Framebuffer& framebuffer, const vk::raii::Pipeline& pipeline, vk::Extent2D extent, const vk::Viewport& viewport, const vk::Rect2D& scissor) const;

        void submit_cmd_buffer(const vk::raii::Fence& fence, const logical_device& logicalDevice) const;

        void present(const logical_device& logicalDevice, const swap_chain& swapChain, std::uint32_t imageIndex) const;
    public:
        command_buffer(const vk::raii::Device& device, vk::raii::CommandBuffer cmdBuffer)
            : m_CommandBuffer{std::move(cmdBuffer)}
            , m_ImageAvailable{device, vk::SemaphoreCreateInfo{}}
            , m_RenderFinished{device, vk::SemaphoreCreateInfo{}}
        { }

        [[nodiscard]]
        vk::Result draw_frame(const vk::raii::Fence& fence,
                              const logical_device& logicalDevice,
                              const swap_chain& swapChain,
                              const vk::raii::RenderPass& renderPass,
                              std::span<const vk::raii::Framebuffer> framebuffers,
                              const vk::raii::Pipeline& pipeline,
                              vk::Extent2D extent,
                              const vk::Viewport& viewport,
                              const vk::Rect2D& scissor) const;
    };

    struct surface {
        vk::raii::SurfaceKHR surfaceKHR;
        vk::PhysicalDeviceSurfaceInfo2KHR info{.surface{surfaceKHR}};
    };

    class presentable {
        std::vector<vk::LayerProperties>     m_LayerProperties;
        std::vector<vk::ExtensionProperties> m_ExtensionProperties;
        vk::raii::Instance                   m_Instance;
        surface                              m_Surface;
        vk::Extent2D                         m_Extent;
        logical_device                       m_LogicalDevice;
        swap_chain                           m_SwapChain;
    public:
        presentable(const presentation_config& presentationConfig, const vk::raii::Context& context, std::function<vk::raii::SurfaceKHR(vk::raii::Instance&)> surfaceCreator, vk::Extent2D framebufferExtent);

        [[nodiscard]]
        std::span<const vk::ExtensionProperties> extension_properties() const noexcept { return m_ExtensionProperties; }

        [[nodiscard]]
        std::span<const vk::LayerProperties> layer_properties() const noexcept { return m_LayerProperties; }

        [[nodiscard]]
        const vk::raii::Instance& instance() const noexcept { return m_Instance; }

        [[nodiscard]]
        const logical_device& get_logical_device() const noexcept { return m_LogicalDevice; }

        [[nodiscard]]
        const swap_chain& get_swap_chain() const noexcept { return m_SwapChain; }

        [[nodiscard]]
        vk::Extent2D extent() const noexcept { return m_Extent; }

        void wait_idle() const;
    };

    class renderer {
        const logical_device*                m_LogicalDevice{};
        const swap_chain*                    m_SwapChain{};
        vk::Extent2D                         m_Extent;
        vk::raii::RenderPass                 m_RenderPass;
        std::vector<vk::raii::Framebuffer>   m_Framebuffers;

        vk::Viewport                         m_ViewPort;
        vk::Rect2D                           m_Scissor;
        vk::raii::PipelineLayout             m_PipelineLayout;
        vk::raii::Pipeline                   m_Pipeline;

        vk::raii::CommandPool                m_CommandPool;
        std::vector<command_buffer>          m_CommandBuffers;
        std::vector<vk::raii::Fence>         m_Fences;
        mutable std::uint32_t                m_CurrentFrameIdx{}, m_CurrentImageIdx{};
    public:
        renderer(const logical_device& logicalDevice, const swap_chain& swapChain, vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, std::uint32_t maxFramesInFlight);

        [[nodiscard]]
        vk::Result draw_frame() const;
    };
}