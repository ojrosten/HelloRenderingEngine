////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Presentation/Presentation.hpp"

#include "avocet/Vulkan/Shaders/Shaders.hpp"

#include <ranges>

namespace avocet::vulkan {
    namespace {
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
                .sType{VK_STRUCTURE_TYPE_APPLICATION_INFO},
                .pApplicationName{config.create_info.app_info.app.name.data()},
                .applicationVersion{config.create_info.app_info.app.version},
                .pEngineName{config.create_info.app_info.engine.name.data()},
                .engineVersion{config.create_info.app_info.engine.version},
                .apiVersion{VK_API_VERSION_1_4}
            };

            vk::InstanceCreateInfo createInfo{
                .sType{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO},
                .flags{config.create_info.flags},
                .pApplicationInfo{&appInfo},
                .enabledLayerCount{static_cast<std::uint32_t>(config.validation_layers.size())},
                .ppEnabledLayerNames{config.validation_layers.data()},
                .enabledExtensionCount{static_cast<std::uint32_t>(config.extensions.size())},
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
        std::vector<vk::raii::ImageView> make_swap_chain_image_views(const vk::raii::Device& device, const swap_chain& swapChain, std::span<const vk::Image> images) {
            return
                std::views::transform(
                    images,
                    [&device, &swapChain](const vk::Image& image) -> vk::raii::ImageView {
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

        [[nodiscard]]
        vk::raii::RenderPass make_render_pass(const logical_device& logicalDevice) {
            vk::AttachmentDescription2 colourAttachment{
                .format{logicalDevice.format()},
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

            vk::RenderPassCreateInfo2 info{
                .attachmentCount{1},
                .pAttachments{&colourAttachment},
                .subpassCount{1},
                .pSubpasses{&subpass}
            };

            return vk::raii::RenderPass{logicalDevice.device(), info};
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
                .dynamicStateCount{static_cast<std::uint32_t>(states.size())},
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
                .blendEnable{}
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
        std::vector<vk::raii::Framebuffer> make_framebuffers(const logical_device& logicalDevice, const vk::raii::RenderPass& renderPass, vk::Extent2D extent) {
            return
                std::views::transform(
                    logicalDevice.swapchain_image_views(),
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
        vk::raii::CommandPool make_command_pool(const logical_device& logicalDevice) {
            vk::CommandPoolCreateInfo info{
                .flags{vk::CommandPoolCreateFlagBits::eResetCommandBuffer},
                .queueFamilyIndex{logicalDevice.q_family_indices().graphics.value()}
            };

            return vk::raii::CommandPool{logicalDevice.device(), info};
        }

        [[nodiscard]]
        std::vector<vk::raii::CommandBuffer> make_command_buffers(const vk::raii::Device& device, const vk::raii::CommandPool& pool) {
            vk::CommandBufferAllocateInfo allocInfo{
                .commandPool{pool},
                .commandBufferCount{1}
            };

            return device.allocateCommandBuffers(allocInfo);
        }
    }

    swap_chain_support_details::swap_chain_support_details(const vk::raii::PhysicalDevice& physDevice, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo, vk::Extent2D framebufferExtent)
        : capabilities{physDevice.getSurfaceCapabilities2KHR(surfaceInfo)}
        , formats{physDevice.getSurfaceFormats2KHR(surfaceInfo)}
        , present_modes{physDevice.getSurfacePresentModesKHR(surfaceInfo.surface)}
        , framebuffer_extent{framebufferExtent}
    {
    }

    logical_device::logical_device(physical_device physDevice, const device_config& deviceConfig, const vk::PhysicalDeviceSurfaceInfo2KHR& surfaceInfo)
        : m_PhysicalDevice{std::move(physDevice)}
        , m_Device{make_logical_device(m_PhysicalDevice, deviceConfig.extensions)}
        , m_GraphicsQueue{m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{m_PhysicalDevice.q_family_indices.graphics.value()}})}
        , m_PresentQueue{m_Device.getQueue2(vk::DeviceQueueInfo2{.queueFamilyIndex{m_PhysicalDevice.q_family_indices.present.value()}})}
        , m_SwapChain{make_swap_chain(m_PhysicalDevice, m_Device, surfaceInfo, deviceConfig.swap_chain, m_PhysicalDevice.swap_chain_details)}
        , m_SwapChainImages{m_SwapChain.chain.getImages()}
        , m_SwapChainImageViews{make_swap_chain_image_views(m_Device, m_SwapChain, m_SwapChainImages)}
    {
    }

    presentable::presentable(const presentation_config& presentationConfig, const vk::raii::Context& context, std::function<vk::raii::SurfaceKHR(vk::raii::Instance&)> surfaceCreator, vk::Extent2D framebufferExtent, const std::filesystem::path& vertShaderPath, const std::filesystem::path& fragShaderPath)
        : m_LayerProperties{vk::enumerateInstanceLayerProperties()}
        , m_ExtensionProperties{vk::enumerateInstanceExtensionProperties()}
        , m_Instance{make_instance(context, check_validation_layer_support(presentationConfig, m_LayerProperties))}
        , m_Surface{surfaceCreator(m_Instance)}
        , m_Extent{framebufferExtent}
        , m_LogicalDevice{
            presentationConfig.device_config.selector(m_Instance.enumeratePhysicalDevices(), presentationConfig.device_config.extensions, vk::PhysicalDeviceSurfaceInfo2KHR{.surface{m_Surface}}, m_Extent),
            presentationConfig.device_config,
            vk::PhysicalDeviceSurfaceInfo2KHR{.surface{m_Surface}}
        },
        m_RenderPass{make_render_pass(m_LogicalDevice)},
        m_Framebuffers{make_framebuffers(m_LogicalDevice, m_RenderPass, m_Extent)},
        m_ViewPort{
            .x{},
            .y{},
            .width {static_cast<float>(m_Extent.width)},
            .height{static_cast<float>(m_Extent.height)},
            .minDepth{},
            .maxDepth{1.0f}
        },
        m_Scissor{
            .offset{},
            .extent{m_Extent}
        },
        m_PipelineLayout{m_LogicalDevice.device(), vk::PipelineLayoutCreateInfo{}},
        m_Pipeline{
            make_pipeline(
                m_LogicalDevice.device(),
                create_shader_module(m_LogicalDevice.device(), vertShaderPath, shaderc_shader_kind::shaderc_glsl_vertex_shader  , vertShaderPath.generic_string()),
                create_shader_module(m_LogicalDevice.device(), fragShaderPath, shaderc_shader_kind::shaderc_glsl_fragment_shader, fragShaderPath.generic_string()),
                m_PipelineLayout,
                m_RenderPass
            )
        },
        m_CommandPool{make_command_pool(m_LogicalDevice)},
        m_CommmandBuffers{make_command_buffers(m_LogicalDevice.device(), m_CommandPool)}
    {
    }

    void presentable::draw(std::uint32_t imageIndex) const {
        vk::CommandBufferBeginInfo bufferBeginInfo{};

        get_cmd_buffer().begin(bufferBeginInfo);

        vk::ClearValue clearCol{{0.0f, 0.0f, 0.0f, 1.0f}};

        vk::RenderPassBeginInfo renderPassInfo{
            .renderPass{m_RenderPass},
            .framebuffer{m_Framebuffers[imageIndex]},
            .renderArea{
                .offset{},
                .extent{m_Extent}
             },
            .clearValueCount{1},
            .pClearValues{&clearCol}
        };

        get_cmd_buffer().beginRenderPass2(renderPassInfo, vk::SubpassBeginInfo{});

        get_cmd_buffer().bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);

        get_cmd_buffer().setViewport(0, m_ViewPort);
        get_cmd_buffer().setScissor(0, m_Scissor);
        get_cmd_buffer().draw(3, 1, 0, 0);

        get_cmd_buffer().endRenderPass2(vk::SubpassEndInfo{});

        get_cmd_buffer().end();
    }
}