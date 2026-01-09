////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Setup/Instance.hpp"

#include "avocet/Vulkan/Common/Casts.hpp"
#include "avocet/Vulkan/Common/Conversions.hpp"

namespace avocet::vulkan {
    namespace {
        const instance_info& check_validation_layer_support(const instance_info& info, std::span<const vk::LayerProperties> layerProperties) {
            for(const auto& requested : info.validation_layers) {
                auto found{std::ranges::find_if(layerProperties, [&requested](std::string_view actualName) { return std::string_view{requested} == actualName; }, [](const vk::LayerProperties& prop) { return prop.layerName.data(); })};
                if(found == layerProperties.end())
                    throw std::runtime_error{std::format("Unable to find requested validation layer {}", requested)};
            }

            return info;
        }

        [[nodiscard]]
        vk::raii::Instance make_instance(const vk::raii::Context& vulkanContext, const instance_info& info) {
            vk::ApplicationInfo appInfo{
                .pApplicationName{info.app_info.app.name.data()},
                .applicationVersion{info.app_info.app.version},
                .pEngineName{info.app_info.engine.name.data()},
                .engineVersion{info.app_info.engine.version},
                .apiVersion{vk::ApiVersion14}
            };

            auto rawValidationLayers{to_cstyle_strings(info.validation_layers)},
                 rawExtensions      {to_cstyle_strings(info.extensions)};


            vk::InstanceCreateInfo createInfo{
                .flags{info.flags},
                .pApplicationInfo{&appInfo},
                .enabledLayerCount{to_uint32(rawValidationLayers.size())},
                .ppEnabledLayerNames{rawValidationLayers.data()},
                .enabledExtensionCount{to_uint32(rawExtensions.size())},
                .ppEnabledExtensionNames{rawExtensions.data()}
            };

            return vk::raii::Instance{vulkanContext, createInfo};
        }
    }

    instance::instance(const vk::raii::Context& context, const instance_info& instanceInfo)
        : m_Context{&context}
        , m_LayerProperties{vk::enumerateInstanceLayerProperties()}
        , m_ExtensionProperties{vk::enumerateInstanceExtensionProperties()}
        , m_Instance{make_instance(context, check_validation_layer_support(instanceInfo, m_LayerProperties))}
    {
    }
}