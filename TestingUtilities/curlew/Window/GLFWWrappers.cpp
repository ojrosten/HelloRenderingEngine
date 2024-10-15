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

namespace {
    void errorCallback(int error, const char* description) {
        std::cerr << std::format("Error - {}: {}\n", error, description);
    }

    GLFWwindow& make_window() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        auto win{glfwCreateWindow(800, 600, "Hello GLFW", nullptr, nullptr)};
        return win ? *win : throw std::runtime_error{"Failed to create GLFW window"};
    }
}

namespace curlew {
    glfw_manager::glfw_manager() {
        glfwSetErrorCallback(errorCallback);

        if(!glfwInit())
            throw std::runtime_error{"Failed to initialize GLFW\n"};
    }

    glfw_manager::~glfw_manager() { glfwTerminate(); }

    window glfw_manager::create_window() { return window{}; }

    window_resource::window_resource() : m_Window{make_window()} {}

    window_resource::~window_resource() { glfwDestroyWindow(&m_Window); }

    window::window() {
        glfwMakeContextCurrent(&m_Window.get());

        if(!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error{"Failed to initialize GLAD"};
    }
}