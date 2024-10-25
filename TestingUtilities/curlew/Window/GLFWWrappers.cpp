////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <format>

namespace curlew {
    namespace {
        void errorCallback(int error, const char* description) {
            std::cerr << std::format("Error - {}: {}\n", error, description);
        }

        GLFWwindow& make_window(const window_config& config) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(config.version.major));
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(config.version.minor));
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            if(config.hidden == window_hiding::yes)
                glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

            auto win{glfwCreateWindow(static_cast<int>(config.width), static_cast<int>(config.height), config.name.data(), nullptr, nullptr)};
            return win ? *win : throw std::runtime_error{"Failed to create GLFW window"};
        }
    }


    glfw_manager::glfw_manager() {
        glfwSetErrorCallback(errorCallback);

        if(!glfwInit())
            throw std::runtime_error{"Failed to initialize GLFW\n"};
    }

    glfw_manager::~glfw_manager() { glfwTerminate(); }

    window glfw_manager::create_window(const window_config& config) { return window{config}; }

    window_resource::window_resource(const window_config& config) : m_Window{make_window(config)} {}

    window_resource::~window_resource() { glfwDestroyWindow(&m_Window); }

    window::window(const window_config& config) : m_Window{config} {
        glfwMakeContextCurrent(&m_Window.get());

        if(!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error{"Failed to initialize GLAD"};
    }
}