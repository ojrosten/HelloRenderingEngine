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
    [[nodiscard]]
    bool is_intel(std::string_view renderer) { return renderer.find("Intel") != std::string::npos; }

    [[nodiscard]]
    bool is_nvidia(std::string_view renderer) { return renderer.find("NVIDIA") != std::string::npos; }

    [[nodiscard]]
    bool is_amd(std::string_view renderer) { return renderer.find("AMD") != std::string::npos; }

    [[nodiscard]]
    bool is_mesa(std::string_view renderer) { return renderer.find("llvmpipe") != std::string::npos; }

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
        std::string operating_system() {
            using namespace avocet;
            if(is_windows()) return "Win";
            if(is_linux())   return "Linux";
            if(is_apple())   return "Apple";

            return "";
        }

        [[nodiscard]]
        std::string concise_renderer(std::string_view rawRenderer) {
            if(is_intel(rawRenderer))  return "Intel";
            if(is_amd(rawRenderer))    return "AMD";
            if(is_nvidia(rawRenderer)) return "NVIDIA";
            if(is_mesa(rawRenderer))   return "Mesa";

            return "";
        }
        
        std::string& add_separator(std::string& str) {
            if(!str.empty()) str += "_";

            return str;
        }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        constexpr bool os_dependent(Flavour flavour) noexcept { return (flavour & Flavour::os) == Flavour::os; }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        constexpr bool renderer_dependent(Flavour flavour) noexcept { return (flavour & Flavour::renderer) == Flavour::renderer; }

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
        std::string do_make_discriminator(Flavour flavour) {
            std::string str{};
            const auto [version, renderer]{glfw_manager{}.find_rendering_setup()};
            if(os_dependent(flavour))
                str += operating_system();

            if(renderer_dependent(flavour))
                add_separator(str) += concise_renderer(renderer);

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
