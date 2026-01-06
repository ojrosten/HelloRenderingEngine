////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Shaders/Shaders.hpp"

#include <span>

namespace avocet::vulkan {
    namespace {
        [[nodiscard]]
        vk::raii::ShaderModule do_create_shader_module(const vk::raii::Device& device, std::span<const std::uint32_t> byteCode) {
            vk::ShaderModuleCreateInfo info{
                .codeSize{byteCode.size()},
                .pCode{byteCode.data()}
            };

            return vk::raii::ShaderModule{device, info};
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

        void foo() {
            vk::PipelineVertexInputStateCreateInfo vertInfo{

            };

            vk::PipelineInputAssemblyStateCreateInfo assemblyInfo{
                .topology{vk::PrimitiveTopology::eTriangleList}
            };
        }
    }

    [[nodiscard]]
    vk::raii::ShaderModule create_shader_module(const vk::raii::Device& device, const std::filesystem::path& shaderPath, shaderc_shader_kind kind, std::string_view label) {
        return do_create_shader_module(device, compile_glsl_to_spv(shaderPath, kind, label));
    }

}