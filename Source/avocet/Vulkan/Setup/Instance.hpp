////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Vulkan/Common/VulkanConfig.hpp"

#include <string>

namespace avocet::vulkan {
    struct product_info {
        std::string   name{};
        std::uint32_t version{vk::makeApiVersion(0, 1, 0, 0)};
    };

    struct application_info {
        product_info app{}, engine{};
    };

    struct instance_info {
        vk::InstanceCreateFlags flags{vk::InstanceCreateFlagBits::eEnumeratePortabilityKHR};
        application_info app_info{};
        std::vector<std::string> validation_layers{};
        std::vector<std::string> extensions{};
    };

    class instance {
        const vk::raii::Context* m_Context{};
        std::vector<vk::LayerProperties>     m_LayerProperties;
        std::vector<vk::ExtensionProperties> m_ExtensionProperties;
        vk::raii::Instance                   m_Instance;
    public:
        instance(const vk::raii::Context& context, const instance_info& instanceInfo);

        [[nodiscard]]
        std::span<const vk::ExtensionProperties> extension_properties() const noexcept { return m_ExtensionProperties; }

        [[nodiscard]]
        std::span<const vk::LayerProperties> layer_properties() const noexcept { return m_LayerProperties; }

        [[nodiscard]]
        const vk::raii::Context& context() const noexcept { return *m_Context; }

        [[nodiscard]]
        const vk::raii::Instance& get() const noexcept { return m_Instance; }
    };
}