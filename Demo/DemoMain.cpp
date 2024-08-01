////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "TestingUtilities/GLFWWrappers.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <iostream>

int main()
{
    try
    {
        demo::glfw_manager manager{};
        auto w{manager.create_window()};

        if(!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error{"Failed to initialize GLAD"};

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glfwSwapBuffers(&w.get());
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


