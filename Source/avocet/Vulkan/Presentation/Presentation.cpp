////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Presentation/Presentation.hpp"

#include "avocet/Vulkan/Shaders/Shaders.hpp"

#include <print>
#include <ranges>

namespace avocet::vulkan {
    namespace {
        constexpr auto maxTimeout{std::numeric_limits<std::uint64_t>::max()};

        [[nodiscard]]
        std::uint32_t to_uint32(std::size_t i) { return static_cast<std::uint32_t>(i); }

        const presentation_config& check_validation_layer_support(const presentation_config& config, std::span<const vk::LayerProperties> layerProperties) {
            for(const auto& requested : config.validation_layers) {
                auto found{std::ranges::find_if(layerProperties, [&requested](std::string_view actualName) { return std::string_view{requested} == actualName; }, [](const vk::LayerProperties& prop) { return prop.layerName.data(); })};
                if(found == layerProperties.end())
                    throw std::runtime_error{std::format("Unable to find requested validation layer {}", requested)};
            }

            return config;
        }

        [[nodiscard]]
        vk::raii::Instance make_instance(const vk::raii::Context& vulkanContext, const presentation_config& config) {
            vk::ApplicationInfo appInfo{
                .pApplicationName{config.create_info.app_info.app.name.data()},
                .applicationVersion{config.create_info.app_info.app.version},
                .pEngineName{config.create_info.app_info.engine.name.data()},
                .engineVersion{config.create_info.app_info.engine.version},
                .apiVersion{VK_API_VERSION_1_4}
            };

            vk::InstanceCreateInfo createInfo{
                .flags{config.create_info.flags},
                .pApplicationInfo{&appInfo},
                .enabledLayerCount{to_uint32(config.validation_layers.size())},
                .ppEnabledLayerNames{config.validation_layers.data()},
                .enabledExtensionCount{to_uint32(config.extensions.size())},
                .ppEnabledExtensionNames{config.extensions.data()}
            };

            return vk::raii::Instance{vulkanContext, createInfo};
        }

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

            vk::DeviceCreateInfo createInfo{
                .pNext{&syncFeatures},
                .queueCreateInfoCount{to_uint32(qCreateInfos.size())},
                .pQueueCreateInfos{qCreateInfos.data()},
                .enabledExtensionCount{to_uint32(extensions.size())},
                .ppEnabledExtensionNames{extensions.data()},
                .pEnabledFeatures{&features}
            };

            return physDevice.device.createDevice(createInfo);
        }

        [[nodiscard]]
        swap_chain_and_format make_swap_chain(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_config& swapChainConfig, const swap_chain_support_details& swapChainDetails) {
            
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
                .imageExtent{swapChainConfig.extent_selector(swapChainDetails.capabilities, swapChainDetails.framebuffer_extent)},
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

        [[nodiscard]]
        vk::raii::RenderPass make_render_pass(const logical_device& logicalDevice, const swap_chain& swapChain) {
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
        std::vector<vk::raii::Framebuffer> make_framebuffers(const logical_device& logicalDevice, const swap_chain& swapChain, const vk::raii::RenderPass& renderPass, vk::Extent2D extent) {
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
        vk::raii::Pipeline make_pipeline(const vk::raii::Device& device, const vk::raii::ShaderModule& vertModule, const vk::raii::ShaderModule& fragModule, const vk::raii::PipelineLayout& pipelineLayout, const vk::raii::RenderPass& renderPass) {
            const std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages{make_pipeline_shader_info(vertModule, fragModule)};

            vk::PipelineVertexInputStateCreateInfo vertInfo{};

            vk::PipelineInputAssemblyStateCreateInfo assemblyInfo{
                .topology{vk::PrimitiveTopology::eTriangleList}
            };

            vk::PipelineViewportStateCreateInfo viewportInfo{
                .viewportCount{1},
                //.pViewports{&m_ViewPort}, These can now be dynamic and not baked into the pipeline
                .scissorCount{1},
                //.pScissors{&m_Scissor}
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
               .renderPass{renderPass},
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
              | std::views::transform([&device](auto) -> vk::raii::Fence { return {device, vk::FenceCreateInfo{.flags{vk::FenceCreateFlagBits::eSignaled}}} ; })
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

    swap_chain_support_details::swap_chain_support_details(const vk::raii::PhysicalDevice& physDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, vk::Extent2D framebufferExtent)
        : capabilities{physDevice.getSurfaceCapabilities2KHR(surfaceInfo)}
        , formats{physDevice.getSurfaceFormats2KHR(surfaceInfo)}
        , present_modes{physDevice.getSurfacePresentModesKHR(surfaceInfo.surface)}
        , framebuffer_extent{framebufferExtent}
    {
    }

    logical_device::logical_device(physical_device physDevice, const device_config& deviceConfig)
        : m_PhysicalDevice{std::move(physDevice)}
        , m_Device{make_logical_device(m_PhysicalDevice, deviceConfig.extensions)}
        , m_GraphicsQueue{m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{m_PhysicalDevice.q_family_indices.graphics.value()}})}
        , m_PresentQueue{m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{m_PhysicalDevice.q_family_indices.present.value()}})}
    {
    }

    swap_chain::swap_chain(const logical_device& logicalDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, const swap_chain_config& swapChainConfig, const swap_chain_support_details& swapChainDetails)
        : m_Config{swapChainConfig}
        , m_Chain{make_swap_chain(logicalDevice, surfaceInfo, m_Config, swapChainDetails)}
        , m_Images{m_Chain.chain.getImages()}
        , m_ImageViews{make_swap_chain_image_views(logicalDevice.device(), m_Chain.format, m_Images)}
    {
    }

    presentable::presentable(const presentation_config& presentationConfig, const vk::raii::Context& context, std::function<vk::raii::SurfaceKHR(vk::raii::Instance&)> surfaceCreator, vk::Extent2D framebufferExtent)
        : m_LayerProperties{vk::enumerateInstanceLayerProperties()}
        , m_ExtensionProperties{vk::enumerateInstanceExtensionProperties()}
        , m_Instance{make_instance(context, check_validation_layer_support(presentationConfig, m_LayerProperties))}
        , m_Surface{surfaceCreator(m_Instance)}
        , m_Extent{framebufferExtent}
        , m_LogicalDevice{
            presentationConfig.device_config.selector(m_Instance.enumeratePhysicalDevices(), presentationConfig.device_config.extensions, m_Surface.info, m_Extent),
            presentationConfig.device_config
          }
        , m_SwapChain{m_LogicalDevice, m_Surface.info, presentationConfig.device_config.swap_chain, m_LogicalDevice.swap_chain_support()}
    {
    }

    void presentable::rebuild_swapchain() {
        wait_idle();
        m_SwapChain = swap_chain{m_LogicalDevice, m_Surface.info, m_SwapChain.config(), m_LogicalDevice.swap_chain_support()};
    }

    void presentable::wait_idle() const {
        m_LogicalDevice.device().waitIdle();
    }

    [[nodiscard]]
    vk::Result command_buffer::draw_frame(const vk::raii::Fence& fence,
                                          const logical_device& logicalDevice,
                                          const swap_chain& swapChain,
                                          const vk::raii::RenderPass& renderPass,
                                          std::span<const vk::raii::Framebuffer> framebuffers,
                                          const vk::raii::Pipeline& pipeline,
                                          vk::Extent2D extent,
                                          const vk::Viewport& viewport,
                                          const vk::Rect2D& scissor) const {
        if(logicalDevice.device().waitForFences(*fence, true, maxTimeout) != vk::Result::eSuccess)
            throw std::runtime_error{"Waiting for fences failed"};


        const auto [ec, imageIndex]{
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
            record_cmd_buffer(renderPass, framebuffers[imageIndex], pipeline, extent, viewport, scissor);
            submit_cmd_buffer(fence, logicalDevice);
            present(logicalDevice, swapChain, imageIndex);
        }

        return ec;
    }

    void command_buffer::record_cmd_buffer(const vk::raii::RenderPass& renderPass, const vk::Framebuffer& framebuffer, const vk::raii::Pipeline& pipeline, vk::Extent2D extent, const vk::Viewport& viewport, const vk::Rect2D& scissor) const {
        command_buffer_sentinel cmdBufferSentinel{m_CommandBuffer, vk::CommandBufferBeginInfo{}};

        vk::ClearValue clearCol{{0.0f, 0.0f, 0.0f, 1.0f}};

        vk::RenderPassBeginInfo renderPassInfo{
            .renderPass{renderPass},
            .framebuffer{framebuffer},
            .renderArea{
                .offset{},
                .extent{extent}
             },
            .clearValueCount{1},
            .pClearValues{&clearCol}
        };

        render_pass_sentinel renderPassSentinel{m_CommandBuffer, renderPassInfo, vk::SubpassBeginInfo{}, vk::SubpassEndInfo{}};

        m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        m_CommandBuffer.setViewport(0, viewport);
        m_CommandBuffer.setScissor(0, scissor);
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

    void command_buffer::present(const avocet::vulkan::logical_device& logicalDevice, const swap_chain& swapChain, std::uint32_t imageIndex) const {
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
            std::println("Presenting the graphics queue returrned result {}", static_cast<int>(e));
    }


    renderer::renderer(const logical_device& logicalDevice, const swap_chain& swapChain, vk::Extent2D extent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight)
        : m_LogicalDevice{&logicalDevice}
        , m_SwapChain{&swapChain}
        , m_Extent{extent}
        , m_RenderPass{make_render_pass(logicalDevice, swapChain)}
        , m_Framebuffers{make_framebuffers(logicalDevice, swapChain, m_RenderPass, m_Extent)}
        , m_ViewPort{
            .x{},
            .y{},
            .width {static_cast<float>(m_Extent.width)},
            .height{static_cast<float>(m_Extent.height)},
            .minDepth{},
            .maxDepth{1.0f}
          }
        , m_Scissor{
            .offset{},
            .extent{m_Extent}
          }
        , m_PipelineLayout{logicalDevice.device(), vk::PipelineLayoutCreateInfo{}}
        , m_Pipeline{
              make_pipeline(
                  logicalDevice.device(),
                  create_shader_module(logicalDevice.device(), vertShaderPath, shaderc_shader_kind::shaderc_glsl_vertex_shader  , vertShaderPath.generic_string()),
                  create_shader_module(logicalDevice.device(), fragShaderPath, shaderc_shader_kind::shaderc_glsl_fragment_shader, fragShaderPath.generic_string()),
                  m_PipelineLayout,
                  m_RenderPass
              )
          }
        , m_CommandPool{make_command_pool(logicalDevice)}
        , m_CommandBuffers{make_command_buffer(logicalDevice.device(), m_CommandPool, to_uint32(swapChain.image_views().size()))}
        , m_Fences{make_fences(logicalDevice.device(), maxFramesInFlight)}
    { }

    void renderer::draw_frame() const {
        auto ec{m_CommandBuffers[m_CurrentImageIdx].draw_frame(m_Fences[m_CurrentFrameIdx], *m_LogicalDevice, *m_SwapChain, m_RenderPass, m_Framebuffers, m_Pipeline, m_Extent, m_ViewPort, m_Scissor)};

        m_CurrentImageIdx = (m_CurrentImageIdx + 1) % m_SwapChain->image_views().size();
        m_CurrentFrameIdx = (m_CurrentFrameIdx + 1) % m_Fences.size();
    }

    void rendering_system::make_renderer(const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath, frames_in_flight maxFramesInFlight) {
        m_Renderers.emplace_back(m_Presentable.get_logical_device(), m_Presentable.get_swap_chain(), m_Presentable.extent(), vertShaderPath, fragShaderPath, maxFramesInFlight);
    }

    void rendering_system::rebuild_swapchain() {
        m_Presentable.rebuild_swapchain();
    }
}