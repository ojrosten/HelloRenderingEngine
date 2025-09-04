////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Utilities/Version.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"

#include <format>
#include <string>
#include <stdexcept>

namespace avocet::opengl{
    [[nodiscard]]
    std::string get_vendor() {
        return {std::bit_cast<const char*>(gl_function{glGetString}(GL_VENDOR))};
    }

    [[nodiscard]]
    std::string get_renderer() {
        return {std::bit_cast<const char*>(gl_function{glGetString}(GL_RENDERER))};
    }

    [[nodiscard]]
    opengl_version get_opengl_version() {
        if(GLAD_GL_VERSION_4_6) return {4, 6};
        if(GLAD_GL_VERSION_4_5) return {4, 5};
        if(GLAD_GL_VERSION_4_4) return {4, 4};
        if(GLAD_GL_VERSION_4_3) return {4, 3};
        if(GLAD_GL_VERSION_4_2) return {4, 2};
        if(GLAD_GL_VERSION_4_1) return {4, 1};
        if(GLAD_GL_VERSION_4_0) return {4, 0};
        if(GLAD_GL_VERSION_3_3) return {3, 3};
        if(GLAD_GL_VERSION_3_2) return {3, 2};
        if(GLAD_GL_VERSION_3_1) return {3, 1};
        if(GLAD_GL_VERSION_3_0) return {3, 0};
        if(GLAD_GL_VERSION_2_1) return {2, 1};
        if(GLAD_GL_VERSION_2_0) return {2, 0};
        if(GLAD_GL_VERSION_1_5) return {1, 5};
        if(GLAD_GL_VERSION_1_4) return {1, 4};
        if(GLAD_GL_VERSION_1_3) return {1, 3};
        if(GLAD_GL_VERSION_1_2) return {1, 2};
        if(GLAD_GL_VERSION_1_1) return {1, 1};

        return {1, 0};
    }
}