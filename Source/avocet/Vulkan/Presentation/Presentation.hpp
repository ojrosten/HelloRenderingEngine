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
        swap_chain_support_details(const vk::raii::PhysicalDevice& physDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo);

        vk::SurfaceCapabilities2KHR        capabilities{};
        std::vector<vk::SurfaceFormat2KHR> formats{};
        std::vector<vk::PresentModeKHR>    present_modes{};
    };

    struct physical_device {
        vk::raii::PhysicalDevice   device;
        queue_family_indices       q_family_indices{};
    };

    struct swap_chain_config {
        std::function<vk::SurfaceFormat2KHR (std::span<const vk::SurfaceFormat2KHR>)> format_selector{};
        std::function<vk::PresentModeKHR (std::span<const vk::PresentModeKHR>)> present_mode_selector{};
        std::function<vk::Extent2D (const vk::SurfaceCapabilities2KHR&, vk::Extent2D)> extent_selector{};

        vk::ImageUsageFlags image_usage_flags{};
    };

    struct device_config {
        std::function<physical_device(std::span<const vk::raii::PhysicalDevice>, std::span<const char* const>, const vk::PhysicalDeviceSurfaceInfo2KHR&)> selector{};
        std::vector<const char*> extensions{};
        swap_chain_config swap_chain{};
    };

    struct frames_in_flight {
        std::uint32_t value{};

        friend constexpr auto operator<=>(const frames_in_flight&, const frames_in_flight&) noexcept = default;
    };

    struct presentation_config {
        std::size_t width{800}, height{600};
        instance_info create_info{};
        std::vector<const char*> validation_layers{};
        std::vector<const char*> extensions{};
        device_config device_config{};
        frames_in_flight max_frames_in_flight{2};
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

        const physical_device& get_physical_device() const noexcept { return m_PhysicalDevice; }
    };

    struct swap_chain_and_format {
        vk::raii::SwapchainKHR chain{nullptr};
        vk::Format             format;
    };

    class swap_chain {
        swap_chain_config                m_Config;
        swap_chain_and_format            m_Chain;
        std::vector<vk::Image>           m_Images;
        std::vector<vk::raii::ImageView> m_ImageViews;
    public:
        swap_chain() = default;
      
        swap_chain(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_config& swapChainConfig, const swap_chain_support_details& swapChainDetails, vk::Extent2D extent);

        [[nodiscard]]
        const swap_chain_config& config() const noexcept { return m_Config; }

        [[nodiscard]]
        const vk::raii::SwapchainKHR& chain() const noexcept { return m_Chain.chain; }

        [[nodiscard]]
        vk::Format format() const noexcept { return m_Chain.format; }

        [[nodiscard]]
        std::span<const vk::raii::ImageView> image_views() const noexcept { return m_ImageViews; }
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
        logical_device                       m_LogicalDevice;
        swap_chain                           m_SwapChain;

        [[nodiscard]]
        swap_chain_support_details extract_swap_chain_support() const;
    public:
        presentable(const presentation_config& presentationConfig, const vk::raii::Context& context, std::function<vk::raii::SurfaceKHR(vk::raii::Instance&)> surfaceCreator, vk::Extent2D extent);

        void rebuild_swapchain(vk::Extent2D extent);

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

        void wait_idle() const;
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
        {
        }

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

    class renderer {
        const logical_device*              m_LogicalDevice{};
        const swap_chain*                  m_SwapChain{};
        vk::Extent2D                       m_Extent;

        vk::raii::RenderPass               m_RenderPass;
        std::vector<vk::raii::Framebuffer> m_Framebuffers;

        vk::Viewport                       m_ViewPort;
        vk::Rect2D                         m_Scissor;
        vk::raii::PipelineLayout           m_PipelineLayout;
        vk::raii::Pipeline                 m_Pipeline;

        vk::raii::CommandPool              m_CommandPool;
        std::vector<command_buffer>        m_CommandBuffers;
        std::vector<vk::raii::Fence>       m_Fences;
        mutable std::uint32_t              m_CurrentFrameIdx{},
                                           m_CurrentImageIdx{};
    public:
        renderer(const logical_device& logicalDevice, const swap_chain& swapChain, vk::Extent2D extent, const vk::raii::ShaderModule& vertShaderModule, const vk::raii::ShaderModule& fragShaderModule, frames_in_flight maxFramesInFlight);

        [[nodiscard]]
        vk::Result draw_frame() const;
    };

    class rendering_system {

        struct full_renderer {
            full_renderer(const presentable& p, vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight);

            vk::raii::ShaderModule vertex, fragment;
            frames_in_flight       max_frames_in_flight;
            renderer r;
        };

        presentable m_Presentable;
        vk::Extent2D m_Extent;
        std::vector<full_renderer> m_Renderers;
        bool m_ScheduleSwpaChainRebuild{};

        [[nodiscard]]
        vk::Result do_draw_all() const;

        void rebuild_swapchain(vk::Extent2D extent);
    public:
        rendering_system(const presentation_config& presentationConfig, const vk::raii::Context& context, std::function<vk::raii::SurfaceKHR(vk::raii::Instance&)> surfaceCreator, vk::Extent2D extent)
            : m_Presentable{presentationConfig, context, surfaceCreator, extent}
            , m_Extent{extent}
        {}

        void make_renderer(vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight);

        [[nodiscard]]
        std::span<const vk::ExtensionProperties> extension_properties() const noexcept { return m_Presentable.extension_properties(); }

        [[nodiscard]]
        std::span<const vk::LayerProperties> layer_properties() const noexcept { return m_Presentable.layer_properties(); }

        [[nodiscard]]
        const vk::raii::Instance& instance() const noexcept { return m_Presentable.instance(); }

        void wait_idle() const { m_Presentable.wait_idle(); }

        void draw_all(vk::Extent2D extent);
    };
}
