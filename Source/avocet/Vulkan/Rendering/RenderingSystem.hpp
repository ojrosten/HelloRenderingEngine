////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Vulkan/Presentation/Presentation.hpp"

namespace avocet::vulkan {
    class render_pass {
        vk::raii::RenderPass               m_RenderPass;
        std::vector<vk::raii::Framebuffer> m_Framebuffers;
    public:
        render_pass(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, vk::Extent2D extent);

        [[nodiscard]]
        const vk::raii::RenderPass& get() const noexcept { return m_RenderPass; }

        [[nodiscard]]
        std::span<const vk::raii::Framebuffer> framebuffers() const noexcept { return m_Framebuffers; }
    };

    class pipeline {
        vk::raii::PipelineLayout m_PipelineLayout;
        vk::raii::Pipeline       m_Pipeline;
    public:
        pipeline(const logical_device& logicalDevice, const render_pass& renderPass, const vk::raii::ShaderModule& vertShaderModule, const vk::raii::ShaderModule& fragShaderModule);

        [[nodiscard]]
        const vk::raii::Pipeline& get() const noexcept { return m_Pipeline; }
    };

    struct render_area {
        vk::Extent2D extent;
        vk::Viewport viewport;
        vk::Rect2D   scissors;
    };

    class command_buffer {
        vk::raii::CommandBuffer m_CommandBuffer;

        vk::raii::Semaphore m_ImageAvailable,
            m_RenderFinished;

        void record_cmd_buffer(const vk::raii::RenderPass& renderPass, const vk::Framebuffer& framebuffer, const pipeline& pipeLine, const render_area& renderArea) const;

        void submit_cmd_buffer(const vk::raii::Fence& fence, const logical_device& logicalDevice) const;

        void present(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, std::uint32_t imageIndex) const;
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
            const swap_chain_plus_images& swapChain,
            const render_pass& renderPass,
            const pipeline& pipeLine,
            const render_area renderArea) const;
    };

    class draw_submitter {
        vk::raii::CommandPool        m_CommandPool;
        std::vector<command_buffer>  m_CommandBuffers;
        std::vector<vk::raii::Fence> m_Fences;
        mutable std::uint32_t        m_CurrentFrameIdx{},
            m_CurrentImageIdx{};
    public:
        draw_submitter(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, frames_in_flight maxFramesInFlight);

        [[nodiscard]]
        vk::Result draw_frame(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, const render_pass& renderPass, const pipeline& pipe, const render_area& renderArea) const;
    };

    class renderer {
        const logical_device* m_LogicalDevice{};
        const swap_chain_plus_images* m_SwapChain{};

        render_pass                   m_RenderPass;
        pipeline                      m_Pipeline;
        render_area                   m_RenderArea;
        draw_submitter                m_Submitter;
    public:
        renderer(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, vk::Extent2D extent, const vk::raii::ShaderModule& vertShaderModule, const vk::raii::ShaderModule& fragShaderModule, frames_in_flight maxFramesInFlight);

        [[nodiscard]]
        vk::Result draw_frame() const;
    };

    class rendering_system {

        struct full_renderer {
            full_renderer(const presentable& p, vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight);

            vk::raii::ShaderModule vertex,
                fragment;
            frames_in_flight       max_frames_in_flight;
            renderer               renderer;
        };

        presentable m_Presentable;
        vk::Extent2D m_Extent;
        std::vector<full_renderer> m_Renderers;
        bool m_ScheduleSwpaChainRebuild{};

        [[nodiscard]]
        vk::Result do_draw_all() const;

        void rebuild_swapchain(vk::Extent2D extent);
    public:
        rendering_system(const instance& vkInstance, const presentation_configuration& presentationConfig, std::function<vk::raii::SurfaceKHR(const instance&)> surfaceCreator, vk::Extent2D extent)
            : m_Presentable{vkInstance, presentationConfig, surfaceCreator, extent}
            , m_Extent{extent}
        {
        }

        void make_renderer(vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight);

        void wait_idle() const { m_Presentable.wait_idle(); }

        void draw_all(vk::Extent2D extent);
    };
}