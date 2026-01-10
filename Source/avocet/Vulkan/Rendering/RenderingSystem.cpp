////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////


#include "avocet/Vulkan/Rendering/RenderingSystem.hpp"

#include "avocet/Vulkan/Common/Casts.hpp"
#include "avocet/Vulkan/Shaders/Shaders.hpp"

#include <ranges>

namespace avocet::vulkan {
    namespace {
        constexpr auto maxTimeout{std::numeric_limits<std::uint64_t>::max()};

        [[nodiscard]]
        vk::raii::RenderPass make_render_pass(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain) {
            vk::AttachmentDescription2 colourAttachment{
                .format{swapChain.format()},
                .loadOp{vk::AttachmentLoadOp::eClear},
                .finalLayout{vk::ImageLayout::ePresentSrcKHR},
            };

            vk::AttachmentReference2 colourAttachmentRef{
                .attachment{},
                .layout{vk::ImageLayout::eColorAttachmentOptimal}
            };

            vk::SubpassDescription2 subpass{
                .colorAttachmentCount{1},
                .pColorAttachments{&colourAttachmentRef}
            };

            vk::SubpassDependency2 subpassDep{
                .srcSubpass{VK_SUBPASS_EXTERNAL},
                .dstSubpass{},
                .srcStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput},
                .dstStageMask{vk::PipelineStageFlagBits::eColorAttachmentOutput},
                .srcAccessMask{},
                .dstAccessMask{vk::AccessFlagBits::eColorAttachmentWrite}
            };

            vk::RenderPassCreateInfo2 info{
                .attachmentCount{1},
                .pAttachments{&colourAttachment},
                .subpassCount{1},
                .pSubpasses{&subpass},
                .dependencyCount{1},
                .pDependencies{&subpassDep},
            };

            return vk::raii::RenderPass{logicalDevice.device(), info};
        }

        [[nodiscard]]
        std::vector<vk::raii::Framebuffer> make_framebuffers(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, const vk::raii::RenderPass& renderPass, vk::Extent2D extent) {
            return
                std::views::transform(
                    swapChain.image_views(),
                    [&](vk::ImageView view) -> vk::raii::Framebuffer {
                        vk::FramebufferCreateInfo info{
                            .renderPass{renderPass},
                            .attachmentCount{1},
                            .pAttachments{&view},
                            .width{extent.width},
                            .height{extent.height},
                            .layers{1}
                        };

                        return {logicalDevice.device(), info};
                    }
                ) | std::ranges::to<std::vector>();
        }

        [[nodiscard]]
        std::array<vk::PipelineShaderStageCreateInfo, 2> make_pipeline_shader_info(const vk::raii::ShaderModule& vertModule, const vk::raii::ShaderModule& fragModule) {
            return {
                vk::PipelineShaderStageCreateInfo{
                    .stage{vk::ShaderStageFlagBits::eVertex},
                    .module{vertModule},
                    .pName{"main"}
                },
                vk::PipelineShaderStageCreateInfo{
                    .stage{vk::ShaderStageFlagBits::eFragment},
                    .module{fragModule},
                    .pName{"main"}
                }
            };
        }

        [[nodiscard]]
        vk::PipelineDynamicStateCreateInfo make_pipeline_dynamic_info(std::span<const vk::DynamicState> states) {
            return vk::PipelineDynamicStateCreateInfo{
                .dynamicStateCount{to_uint32(states.size())},
                .pDynamicStates{states.data()}
            };
        }

        [[nodiscard]]
        vk::raii::Pipeline make_pipeline(const vk::raii::Device& device, const vk::raii::ShaderModule& vertModule, const vk::raii::ShaderModule& fragModule, const vk::raii::PipelineLayout& pipelineLayout, const render_pass& renderPass) {
            const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages{make_pipeline_shader_info(vertModule, fragModule)};

            vk::PipelineVertexInputStateCreateInfo vertInfo{};

            vk::PipelineInputAssemblyStateCreateInfo assemblyInfo{
                .topology{vk::PrimitiveTopology::eTriangleList}
            };

            vk::PipelineViewportStateCreateInfo viewportInfo{
                .viewportCount{1},
                .scissorCount{1},
            };

            vk::PipelineRasterizationStateCreateInfo rasterInfo{
                .lineWidth{1.0}
            };

            vk::PipelineMultisampleStateCreateInfo multismapleInfo{

            };

            vk::PipelineColorBlendAttachmentState blendState{
                .blendEnable{},
                .colorWriteMask{vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA}
            };

            vk::PipelineColorBlendStateCreateInfo blendInfo{
                .attachmentCount{1},
                .pAttachments{&blendState}
            };

            std::array<vk::DynamicState, 2> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
            const auto dynamicInfo{make_pipeline_dynamic_info(dynamicStates)};

            vk::GraphicsPipelineCreateInfo info{
               .stageCount{2},
               .pStages{shaderStages.data()},
               .pVertexInputState{&vertInfo},
               .pInputAssemblyState{&assemblyInfo},
               .pViewportState{&viewportInfo},
               .pRasterizationState{&rasterInfo},
               .pMultisampleState{&multismapleInfo},
               .pDepthStencilState{},
               .pColorBlendState{&blendInfo},
               .pDynamicState{&dynamicInfo},
               .layout{pipelineLayout},
               .renderPass{renderPass.get()},
               .subpass{}
            };

            return {device, nullptr, info};
        }

        [[nodiscard]]
        vk::raii::CommandPool make_command_pool(const logical_device& logicalDevice) {
            vk::CommandPoolCreateInfo info{
                .flags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
                .queueFamilyIndex{logicalDevice.q_family_indices().graphics.value()}
            };

            return vk::raii::CommandPool{logicalDevice.device(), info};
        }

        [[nodiscard]]
        std::vector<command_buffer> make_command_buffer(const vk::raii::Device& device, const vk::raii::CommandPool& pool, std::uint32_t num) {
            vk::CommandBufferAllocateInfo allocInfo{
                .commandPool{pool},
                .commandBufferCount{num}
            };

            return
                device.allocateCommandBuffers(allocInfo)
                | std::views::as_rvalue
                | std::views::transform([&device](vk::raii::CommandBuffer&& commandBuffer) -> command_buffer { return {device, std::move(commandBuffer)}; })
                | std::ranges::to<std::vector>();
        }

        [[nodiscard]]
        std::vector<vk::raii::Fence> make_fences(const vk::raii::Device& device, const frames_in_flight num) {
            return
                std::views::iota(std::uint32_t{}, num.value)
                | std::views::transform([&device](auto) -> vk::raii::Fence { return {device, vk::FenceCreateInfo{.flags{vk::FenceCreateFlagBits::eSignaled}}}; })
                | std::ranges::to<std::vector>();
        }

        class [[nodiscard]] command_buffer_sentinel {
            const vk::raii::CommandBuffer& m_Buffer;
        public:
            command_buffer_sentinel(const vk::raii::CommandBuffer& buffer, const vk::CommandBufferBeginInfo& bufferBeginInfo)
                : m_Buffer{buffer}
            {
                m_Buffer.begin(bufferBeginInfo);
            }

            ~command_buffer_sentinel() { m_Buffer.end(); }
        };

        class [[nodiscard]] render_pass_sentinel {
            const vk::raii::CommandBuffer& m_Buffer;
            vk::SubpassEndInfo m_SubPassEndInfo;
        public:
            render_pass_sentinel(const vk::raii::CommandBuffer& buffer, const vk::RenderPassBeginInfo& renderPassBeginInfo, const vk::SubpassBeginInfo& subPassBeginInfo, const vk::SubpassEndInfo& subPassEndInfo)
                : m_Buffer{buffer}
                , m_SubPassEndInfo{subPassEndInfo}
            {
                m_Buffer.beginRenderPass2(renderPassBeginInfo, subPassBeginInfo);
            }

            ~render_pass_sentinel() { m_Buffer.endRenderPass2(m_SubPassEndInfo); }
        };
    }

    render_pass::render_pass(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, vk::Extent2D extent)
        : m_RenderPass{make_render_pass(logicalDevice, swapChain)}
        , m_Framebuffers{make_framebuffers(logicalDevice, swapChain, m_RenderPass, extent)}
    {
    }

    pipeline::pipeline(const logical_device& logicalDevice, const render_pass& renderPass, const vk::raii::ShaderModule& vertShaderModule, const vk::raii::ShaderModule& fragShaderModule)
        : m_PipelineLayout{logicalDevice.device(), vk::PipelineLayoutCreateInfo{}}
        , m_Pipeline{
              make_pipeline(
                  logicalDevice.device(),
                  vertShaderModule,
                  fragShaderModule,
                  m_PipelineLayout,
                  renderPass
              )
        }
    {
    }

    void command_buffer::record_cmd_buffer(const vk::raii::RenderPass& renderPass, const vk::Framebuffer& framebuffer, const pipeline& pipeLine, const render_area& renderArea) const {
        command_buffer_sentinel cmdBufferSentinel{m_CommandBuffer, vk::CommandBufferBeginInfo{}};

        vk::ClearValue clearCol{{0.0f, 0.0f, 0.0f, 1.0f}};

        vk::RenderPassBeginInfo renderPassInfo{
            .renderPass{renderPass},
            .framebuffer{framebuffer},
            .renderArea{
                .offset{},
                .extent{renderArea.extent}
             },
            .clearValueCount{1},
            .pClearValues{&clearCol}
        };

        render_pass_sentinel renderPassSentinel{m_CommandBuffer, renderPassInfo, vk::SubpassBeginInfo{}, vk::SubpassEndInfo{}};

        m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeLine.get());
        m_CommandBuffer.setViewport(0, renderArea.viewport);
        m_CommandBuffer.setScissor(0, renderArea.scissors);
        m_CommandBuffer.draw(3, 1, 0, 0);
    }

    void command_buffer::submit_cmd_buffer(const vk::raii::Fence& fence, const avocet::vulkan::logical_device& logicalDevice) const {
        vk::SemaphoreSubmitInfo waitSemInfo{
            .semaphore{m_ImageAvailable},
            .stageMask{vk::PipelineStageFlagBits2::eColorAttachmentOutput},
            .deviceIndex{0}
        };

        vk::CommandBufferSubmitInfo cmdBufInfo{
            .commandBuffer{m_CommandBuffer}
        };

        vk::SemaphoreSubmitInfo sigSemInfo{
            .semaphore{m_RenderFinished}
        };

        vk::SubmitInfo2 info{
            .waitSemaphoreInfoCount{1},
            .pWaitSemaphoreInfos{&waitSemInfo},
            .commandBufferInfoCount{1},
            .pCommandBufferInfos{&cmdBufInfo},
            .signalSemaphoreInfoCount{1},
            .pSignalSemaphoreInfos{&sigSemInfo}
        };

        logicalDevice.get_graphics_queue().submit2(info, fence);
    }

    void command_buffer::present(const avocet::vulkan::logical_device& logicalDevice, const swap_chain_plus_images& swapChain, std::uint32_t imageIndex) const {
        std::array<vk::Semaphore, 1> waitSems{*m_RenderFinished};
        std::array<vk::SwapchainKHR, 1> swapChains{*swapChain.chain()};
        vk::PresentInfoKHR presentInfo{
            .waitSemaphoreCount{1},
            .pWaitSemaphores{waitSems.data()},
            .swapchainCount{1},
            .pSwapchains{swapChains.data()},
            .pImageIndices{&imageIndex}
        };

        if(auto e{logicalDevice.get_graphics_queue().presentKHR(presentInfo)}; e != vk::Result::eSuccess)
            throw std::runtime_error{std::format("Presenting the graphics queue returned error code {}", static_cast<int>(e))};
    }

    [[nodiscard]]
    vk::Result command_buffer::draw_frame(const vk::raii::Fence& fence,
        const logical_device& logicalDevice,
        const swap_chain_plus_images& swapChain,
        const render_pass& renderPass,
        const pipeline& pipeLine,
        const render_area renderArea) const {
        if(logicalDevice.device().waitForFences(*fence, true, maxTimeout) != vk::Result::eSuccess)
            throw std::runtime_error{"Waiting for fences failed"};


        const auto [ec, imageIndex] {
            logicalDevice.device().acquireNextImage2KHR(
                vk::AcquireNextImageInfoKHR{
                    .swapchain{swapChain.chain()},
                    .timeout{maxTimeout},
                    .semaphore{m_ImageAvailable},
                    .deviceMask{1}
                }
            )
            };

        if(ec == vk::Result::eSuccess) {
            logicalDevice.device().resetFences(*fence);

            m_CommandBuffer.reset();
            record_cmd_buffer(renderPass.get(), renderPass.framebuffers()[imageIndex], pipeLine, renderArea);
            submit_cmd_buffer(fence, logicalDevice);
            present(logicalDevice, swapChain, imageIndex);
        }

        return ec;
    }

    draw_submitter::draw_submitter(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, frames_in_flight maxFramesInFlight)
        : m_CommandPool{make_command_pool(logicalDevice)}
        , m_CommandBuffers{make_command_buffer(logicalDevice.device(), m_CommandPool, to_uint32(swapChain.image_views().size()))}
        , m_Fences{make_fences(logicalDevice.device(), maxFramesInFlight)}
    {
    }

    frame_renderer::frame_renderer(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, vk::Extent2D extent, const vk::raii::ShaderModule& vertShaderModule, const vk::raii::ShaderModule& fragShaderModule, frames_in_flight maxFramesInFlight)
        : m_LogicalDevice{&logicalDevice}
        , m_SwapChain{&swapChain}
        , m_RenderPass{logicalDevice, swapChain, extent}
        , m_Pipeline{logicalDevice, m_RenderPass, vertShaderModule, fragShaderModule}
        , m_RenderArea{
            .extent{extent},
            .viewport{
                .x{},
                .y{},
                .width {static_cast<float>(extent.width)},
                .height{static_cast<float>(extent.height)},
                .minDepth{},
                .maxDepth{1.0f}
             },
            .scissors{
                .offset{},
                .extent{extent}
             }
        }
        , m_Submitter{*m_LogicalDevice, *m_SwapChain, maxFramesInFlight}
    {
    }

    [[nodiscard]]
    vk::Result draw_submitter::draw_frame(const logical_device& logicalDevice, const swap_chain_plus_images& swapChain, const render_pass& renderPass, const pipeline& pipe, const render_area& renderArea) const {
        auto ec{m_CommandBuffers[m_CurrentImageIdx].draw_frame(m_Fences[m_CurrentFrameIdx], logicalDevice, swapChain, renderPass, pipe, renderArea)};

        m_CurrentImageIdx = (m_CurrentImageIdx + 1) % swapChain.image_views().size();
        m_CurrentFrameIdx = (m_CurrentFrameIdx + 1) % m_Fences.size();

        return ec;
    }

    [[nodiscard]]
    vk::Result frame_renderer::draw_frame() const { return m_Submitter.draw_frame(*m_LogicalDevice, *m_SwapChain, m_RenderPass, m_Pipeline, m_RenderArea); }

    rendering_system::full_renderer::full_renderer(const presentable& p, vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight)
        : vertex{create_shader_module(p.get_logical_device().device(), vertShaderPath, shaderc_shader_kind::shaderc_glsl_vertex_shader  , vertShaderPath.generic_string())}
        , fragment{create_shader_module(p.get_logical_device().device(), fragShaderPath, shaderc_shader_kind::shaderc_glsl_fragment_shader, fragShaderPath.generic_string())}
        , max_frames_in_flight{maxFramesInFlight}
        , renderer{p.get_logical_device(), p.get_swap_chain(), extent, vertex, fragment, max_frames_in_flight}
    {
    }

    void rendering_system::make_renderer(vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight) {
        m_Renderers.emplace_back(m_Presentable, extent, vertShaderPath, fragShaderPath, maxFramesInFlight);
    }

    void rendering_system::rebuild_swapchain(vk::Extent2D extent) {
        m_Presentable.rebuild_swapchain(extent);
        for(auto& fullRenderer : m_Renderers) {
            fullRenderer.renderer = frame_renderer{m_Presentable.get_logical_device(), m_Presentable.get_swap_chain(), extent, fullRenderer.vertex, fullRenderer.fragment, fullRenderer.max_frames_in_flight};
        }
    }

    void rendering_system::draw_all(vk::Extent2D extent) {
        if(m_ScheduleSwpaChainRebuild || (m_Extent != extent)) {
            rebuild_swapchain(extent);
            m_ScheduleSwpaChainRebuild = false;
            m_Extent = extent;
        }

        if(auto ec{do_draw_all()}; (ec == vk::Result::eErrorOutOfDateKHR) || (ec == vk::Result::eSuboptimalKHR)) {
            m_ScheduleSwpaChainRebuild = true;

        }
        else if(ec != vk::Result::eSuccess) {
            throw std::runtime_error{std::format("Unable to continue rendering due to failure to acquire next swap chain image, error code {}", static_cast<int>(ec))};
        }
    }

    [[nodiscard]]
    vk::Result rendering_system::do_draw_all() const {
        for(const auto& fullRenderer : m_Renderers) {
            if(auto ec{fullRenderer.renderer.draw_frame()}; ec != vk::Result::eSuccess)
                return ec;
        }

        return vk::Result::eSuccess;
    }
}