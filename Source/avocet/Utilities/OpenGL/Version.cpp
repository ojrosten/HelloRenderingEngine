////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Utilities/OpenGL/Version.hpp"

#include"glad/gl.h"

#include <format>
#include <string>
#include <stdexcept>

namespace avocet::opengl{
    [[nodiscard]]
    std::string get_opengl_vendor_string() {
        return {std::bit_cast<const char*>(glGetString(GL_VENDOR))};
    }

    [[nodiscard]]
    std::string get_opengl_renderer_string() {
        return {std::bit_cast<const char*>(glGetString(GL_RENDERER))};
    }

    [[nodiscard]]
    std::string get_opengl_version_string() {
        return {std::bit_cast<const char*>(glGetString(GL_VERSION))};
    }

    namespace {
        [[nodiscard]]
        std::size_t to_digit(const std::string& version, std::string::size_type pos) {
            return static_cast<std::size_t>(std::stoi(version.substr(pos, pos + 1)));
        }

        [[nodiscard]]
        opengl_version find_opengl_version() {
            const std::string version{get_opengl_version_string()};
            const auto npos{std::string::npos};
            if(const auto pos{version.find_first_of('.')}; pos != npos) {
                return {to_digit(version, pos - 1), to_digit(version, pos + 1)};
            }

            throw std::runtime_error{std::format("Unable to divine OpenGL version number from: {}", version)};
        }
    }

    [[nodiscard]]
    opengl_version get_opengl_version() {
        const static opengl_version version{find_opengl_version()};
        return version;
    }
}