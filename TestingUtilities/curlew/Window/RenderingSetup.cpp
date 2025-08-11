////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/RenderingSetup.hpp"

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
            if(is_amd(rawRenderer))      return "AMD";
            if(is_apple(rawRenderer))    return "Apple";
            if(is_intel(rawRenderer))    return "Intel";
            if(is_llvmpipe(rawRenderer)) return "llvmpipe";
            if(is_mesa(rawRenderer))     return "Mesa";
            if(is_nvidia(rawRenderer))   return "NVIDIA";

            return "";
        }

        template<class Flavour>
            requires(std::is_scoped_enum_v<Flavour>)
        [[nodiscard]]
        std::string do_make_discriminator(rendering_setup setup, Flavour flavour) {
            std::string str{};
            if(os_dependent(flavour))
                str += operating_system();

            if(renderer_dependent(flavour))
                add_separator(str) += concise_renderer(setup.renderer);

            if(ogl_version_dependent(flavour)) {
                add_separator(str) += std::format("OpenGL_{}_{}", setup.version.major, setup.version.minor);
            }

            if(build_dependent(flavour)) {
                add_separator(str) += (avocet::has_ndebug() ? "Release" : "Debug");
            }

            return str;
        }
    }

    [[nodiscard]]
    bool is_amd(std::string_view renderer)      { return renderer.find("AMD") != std::string::npos; }

    [[nodiscard]]
    bool is_apple(std::string_view renderer)    { return renderer.find("Apple") != std::string::npos; }

    [[nodiscard]]
    bool is_intel(std::string_view renderer)    { return renderer.find("Intel") != std::string::npos; }

    [[nodiscard]]
    bool is_llvmpipe(std::string_view renderer) { return renderer.find("llvmpipe") != std::string::npos; }

    [[nodiscard]]
    bool is_mesa(std::string_view renderer)     { return renderer.find("Mesa") != std::string::npos; }

    [[nodiscard]]
    bool is_nvidia(std::string_view renderer)   { return renderer.find("NVIDIA") != std::string::npos; }


    [[nodiscard]]
    std::string rendering_setup_discriminator(rendering_setup setup, selectivity_flavour selectivity) { return do_make_discriminator(setup, selectivity); }

    [[nodiscard]]
    std::string rendering_setup_discriminator(rendering_setup setup, specificity_flavour specificity) { return do_make_discriminator(setup, specificity); }

    [[nodiscard]]
    std::string rendering_setup_summary(const rendering_setup& setup) {
        return std::format("GL Vendor  : {}\nGL Renderer: {}\nGL Version : {}.{}\n", setup.vendor, setup.renderer, setup.version.major, setup.version.minor);
    }
}
