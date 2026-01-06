////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Vulkan/Common/VulkanConfig.hpp"

#include "avocet/Core/ShaderCompilation/ShaderCompiler.hpp"

namespace avocet::vulkan {
    [[nodiscard]]
    vk::raii::ShaderModule create_shader_module(const vk::raii::Device& device, const std::filesystem::path& shaderPath, shaderc_shader_kind kind, std::string_view label);
}