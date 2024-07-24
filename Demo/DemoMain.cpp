////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "GLFW/glfw3.h"

#include <iostream>
#include <format>

void errorCallback(int error, const char* description) {
    std::cerr << std::format("Error - {}: {}\n", error, description);
}

struct glfw_manager {
    glfw_manager() {
        glfwSetErrorCallback(errorCallback);

        if(!glfwInit())
            throw std::runtime_error{"Failed to initialize GLFW\n"};
    }

    glfw_manager(const glfw_manager&) = delete;

    glfw_manager& operator=(const glfw_manager&) = delete;

    ~glfw_manager() { glfwTerminate(); }
};

class window {
    GLFWwindow* m_Window{};
public:
    window() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        m_Window = glfwCreateWindow(800, 600, "Hello GLFW", nullptr, nullptr);
        if(!m_Window)
            throw std::runtime_error{"Failed to create GLFW window"};

        glfwMakeContextCurrent(m_Window);
    }

    window(const window&) = delete;

    window& operator=(const window&) = delete;

    ~window() { glfwDestroyWindow(m_Window); }

    GLFWwindow& get() {
        if(!m_Window) throw std::runtime_error{"No window!"};
        return *m_Window;
    }
};

int main()
{
    try
    {
        glfw_manager manager{};
        window w{};

        while(!glfwWindowShouldClose(&w.get())) {
            // Render here (optional)
            //glClear(GL_COLOR_BUFFER_BIT);

            // Swap front and back buffers
            glfwSwapBuffers(&w.get());

            // Poll for and process events
            glfwPollEvents();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what();
    }
    catch(...)
    {
        std::cerr << "Unrecognized error\n";
    }
}


