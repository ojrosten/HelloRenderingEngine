////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////


#include "curlew/TestFramework/GraphicsTestCore.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

#include <algorithm>

namespace curlew {
    namespace {
        std::string& erase_backwards(std::string& message, std::string_view rightPattern, std::string_view leftPattern) {
            constexpr auto npos{std::string::npos};
            if(const auto rightPos{message.find(rightPattern)}; rightPos < npos) {
                if(const auto leftPos{message.rfind(leftPattern, rightPos)}; leftPos < rightPos)
                    message.erase(leftPos + 1, rightPos - leftPos - 1);
            }

            return message;
        }

        [[nodiscard]]
        std::string platform() {
            using namespace avocet;
            if(is_windows()) return "Win_";
            if(is_linux())   return "Linux_";
            if(is_apple())   return "Apple_";

            return "";
        }

        [[nodiscard]]
        std::string manufacturer(std::string_view rawVendor) {
            if(rawVendor.find("Intel") != std::string::npos) return "Intel_";
            if(rawVendor.find("AMD") != std::string::npos) return "AMD_";

            return "";
        }
    }

    [[nodiscard]]
    std::string exception_postprocessor::operator()(std::string message) const {
        erase_backwards(message, "Tests", " ");
        erase_backwards(message, "Source/avocet", " ");

        return message;
    }

    [[nodiscard]]
    std::string get_platform() {
        const auto[version, renderer]{glfw_manager{}.find_rendering_setup()};
        return std::format("{}{}OpenGL_{}_{}", platform(), manufacturer(renderer), version.major, version.minor);
    }

    [[nodiscard]]
    std::string get_build() { return avocet::has_ndebug() ? "Release" : ""; }
}