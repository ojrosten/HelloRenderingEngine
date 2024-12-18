////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/RenderingSetup.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

#include <format>

namespace curlew {
    namespace {
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

        [[nodiscard]]
        std::string operating_system() {
            if(avocet::is_windows()) return "Win";
            if(avocet::is_linux())   return "Linux";
            if(avocet::is_apple())   return "Apple";

            return "";
        }

        [[nodiscard]]
        std::string concise_renderer(std::string_view rawRenderer) {
            if(is_intel(rawRenderer))  return "Intel";
            if(is_amd(rawRenderer))    return "AMD";
            if(is_nvidia(rawRenderer)) return "NVIDIA";
            if(is_mesa(rawRenderer))   return "Mesa";
            if(is_apple(rawRenderer))  return "Apple";

            return "";
        }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        std::string do_make_discriminator(Flavour flavour) {
            std::string str{};
            const auto [version, renderer]{glfw_manager::find_rendering_setup()};
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
    bool is_intel(std::string_view renderer) { return renderer.find("Intel") != std::string::npos; }

    [[nodiscard]]
    bool is_nvidia(std::string_view renderer) { return renderer.find("NVIDIA") != std::string::npos; }

    [[nodiscard]]
    bool is_amd(std::string_view renderer) { return renderer.find("AMD") != std::string::npos; }

    [[nodiscard]]
    bool is_mesa(std::string_view renderer) { return renderer.find("llvmpipe") != std::string::npos; }

    [[nodiscard]]
    bool is_apple(std::string_view renderer) { return renderer.find("Apple") != std::string::npos; }

    [[nodiscard]]
    std::string rendering_setup_discriminator(selectivity_flavour selectivity) { return do_make_discriminator(selectivity); }

    [[nodiscard]]
    std::string rendering_setup_discriminator(specificity_flavour specificity) { return do_make_discriminator(specificity); }

    [[nodiscard]]
    std::string rendering_setup_summary() {
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{curlew::window_hiding_mode::on}})};
        namespace agl = avocet::opengl;
        return std::format("GL Vendor  : {}\nGL Renderer: {}\nGL Version : {}\n", agl::get_vendor(), agl::get_renderer(), agl::get_opengl_version_string());
    }
}