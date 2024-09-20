////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "TestingUtilities/GLFWWrappers.hpp"

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

#include "GLFW/glfw3.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <source_location>

namespace fs = std::filesystem;

[[nodiscard]]
fs::path get_shader_directory() {
    if(fs::path file{std::source_location::current().file_name()}; file.is_absolute()) {
        if(fs::path dir{file.parent_path() / "Shaders"}; fs::exists(dir)) {
            return dir;
        }
        else {
            throw std::runtime_error{std::format("Unable to find shader directory: {}", dir.generic_string())};
        }
    }

    throw std::runtime_error{"Relative file paths not supported!"};
}

[[nodiscard]]
std::string file_to_string(const fs::path& file) {
    if(std::ifstream ifile{file}) {
        return std::string(std::istreambuf_iterator<char>(ifile), {});
    }

    throw std::runtime_error{std::format("Unable to open file {}", file.generic_string())};
}


int main()
{
    try
    {
        demo::glfw_manager manager{};
        auto w{manager.create_window()};

        namespace agl = avocet::opengl;

        const auto shaderDir{get_shader_directory()};
        agl::shader_program shaderProgram{file_to_string(shaderDir / "Identity.vs"), file_to_string(shaderDir / "Monochrome.fs")};

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left  
             0.5f, -0.5f, 0.0f, // right 
             0.0f,  0.5f, 0.0f  // top   
        };

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // draw our first triangle
            shaderProgram.use();
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(&w.get());
            glfwPollEvents();
        }

        // de-allocate all resources once they've outlived their purpose:
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
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


