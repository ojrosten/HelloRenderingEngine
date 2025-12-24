////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Vulkan/Common/VulkanConfig.hpp"

#include <format>

namespace std {
    template<>
    struct formatter<vk::PhysicalDeviceProperties> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(const vk::PhysicalDeviceProperties& properties, auto& ctx) const {
            return
                format_to(
                    ctx.out(),
                    "API Version {}\nDriver Version {}\nVendor ID {}\nDevice ID {}\nDevice Type {}\nDevice Name {}",
                    properties.apiVersion,
                    properties.driverVersion,
                    properties.vendorID,
                    properties.deviceID,
                    properties.deviceType,
                    properties.deviceName.data()
                    // TO DO
                );
        }
    };

    template<>
    struct formatter<vk::PhysicalDeviceFeatures> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(const vk::PhysicalDeviceFeatures& features, auto& ctx) const {
            return
                format_to(
                    ctx.out(),
                    "Geometry Shader {}",
                    static_cast<bool>(features.geometryShader)
                    // TO DO
                );
        }
    };
}