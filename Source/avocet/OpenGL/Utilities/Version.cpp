////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Utilities/Version.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/ContextResolver.hpp"

#include <format>
#include <string>
#include <stdexcept>

namespace avocet::opengl{
    [[nodiscard]]
    std::string get_vendor() {
        if(auto* context = get_current_context()) {
            if(context->GetString) {
                auto result = context->GetString(GL_VENDOR);
                if(result) {
                    return {std::bit_cast<const char*>(result)};
                }
            }
        }
        return "Unknown";
    }

    [[nodiscard]]
    std::string get_renderer() {
        if(auto* context = get_current_context()) {
            if(context->GetString) {
                auto result = context->GetString(GL_RENDERER);
                if(result) {
                    return {std::bit_cast<const char*>(result)};
                }
            }
        }
        return "Unknown";
    }

    [[nodiscard]]
    opengl_version get_opengl_version() {
        if(auto* context = get_current_context()) {
            if(context->GetString) {
                auto result = context->GetString(GL_VERSION);
                if(result) {
                    const char* version_str = std::bit_cast<const char*>(result);
                    int major = 1, minor = 0;
                    if(std::sscanf(version_str, "%d.%d", &major, &minor) >= 1) {
                        return {static_cast<std::size_t>(major), static_cast<std::size_t>(minor)};
                    }
                }
            }
        }
        
        return {1, 0};
    }
}