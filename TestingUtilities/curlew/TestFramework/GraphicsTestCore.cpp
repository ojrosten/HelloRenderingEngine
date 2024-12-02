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
            if(is_windows()) return "Win";
            if(is_linux())   return "Linux";
            if(is_apple())   return "Apple";

            return "";
        }

        [[nodiscard]]
        std::string manufacturer(std::string_view rawVendor) {
            if(rawVendor.find("Intel")    != std::string::npos) return "Intel";
            if(rawVendor.find("AMD")      != std::string::npos) return "AMD";
            if(rawVendor.find("NVIDIA")   != std::string::npos) return "NVIDIA";
	    if(rawVendor.find("llvmpipe") != std::string::npos) return "Mesa";

            return "";
        }
        
        std::string& add_separator(std::string& str) {
            if(!str.empty()) str += "_";

            return str;
        }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        constexpr bool build_dependent(Flavour flavour) noexcept { return (flavour & Flavour::build) == Flavour::build; }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        constexpr bool ogl_version_dependent(Flavour flavour) noexcept { return (flavour & Flavour::opengl_version) == Flavour::opengl_version; }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        constexpr bool hardware_dependent(Flavour flavour) noexcept { return (flavour & Flavour::hardware) == Flavour::hardware; }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        std::string do_make_discriminator(Flavour flavour) {
            std::string str{};
            const auto [version, renderer]{glfw_manager{}.find_rendering_setup()};
            if(hardware_dependent(flavour))
                add_separator(str += platform()) += manufacturer(renderer);

            if(ogl_version_dependent(flavour)) {
                add_separator(str) += std::format("OpenGL_{}_{}", version.major, version.minor);
            }

            if(build_dependent(flavour)) {
                add_separator(str) += (avocet::has_ndebug() ? "Release" : "Debug");
            }

            return str;
        }

    }

    [[nodiscard]]
    std::string exception_postprocessor::operator()(std::string message) const {
        erase_backwards(message, "Tests", " ");
        erase_backwards(message, "Source/avocet", " ");

        return message;
    }

    [[nodiscard]]
    std::string make_discriminator(selectivity_flavour selectivity) { return do_make_discriminator(selectivity); }

    [[nodiscard]]
    std::string make_discriminator(specificity_flavour specificity) { return do_make_discriminator(specificity); }

}
