////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "TestingUtilities/GLFWWrappers.hpp"

#include "avocet/Graphics/OpenGL/ShaderProgram.hpp"

#include "GLFW/glfw3.h"

#include <array>
#include <format>
#include <iostream>
#include <source_location>

namespace fs = std::filesystem;

[[nodiscard]]
fs::path shader_directory() {
    if(const fs::path file{std::source_location::current().file_name()}; file.is_absolute()) {
        if(const auto dir{file.parent_path() / "Shaders"}; fs::exists(dir))
            return dir;
        else
            throw std::runtime_error{std::format("Unable to find shader directory {}", dir.generic_string())};
    }

    throw std::runtime_error{"Relative file paths are not supported!"};
}

namespace avocet::opengl {


    struct vbo_lifecycle_events {
        template<std::size_t N>
        static void generate(gl_index_array<N>& indices)      { glGenBuffers(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const gl_index_array<N>& indices) { glDeleteBuffers(N, indices.data()); }
    };


    struct vao_lifecycle_events {
        template<std::size_t N>
        static void generate(gl_index_array<N>& indices)      { glGenVertexArrays(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const gl_index_array<N>& indices) { glDeleteVertexArrays(N, indices.data()); }
    };

    using vbo_resource = resource<num_resources{1}, vbo_lifecycle_events>;
    using vao_resource = resource<num_resources{1}, vao_lifecycle_events>;

    class triangle {
        std::array<float, 9> m_Vertices{
            -0.5f, -0.5f, 0.0f, // left  
             0.5f, -0.5f, 0.0f, // right 
             0.0f,  0.5f, 0.0f  // top   
        };

        vao_resource m_VAO;
        vbo_resource m_VBO;
    public:
        triangle() {
            glBindVertexArray(m_VAO.handle().index());

            glBindBuffer(GL_ARRAY_BUFFER, m_VBO.handle().index());
            glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }

        void draw() {
            glBindVertexArray(m_VAO.handle().index());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    };
}

int main()
{
    try
    {
        demo::glfw_manager manager{};
        auto w{manager.create_window()};

        namespace agl = avocet::opengl;

        agl::shader_program shaderProgram{shader_directory() / "Identity.vs", shader_directory() / "Monochrome.fs"};

        agl::triangle tri{};

        while(!glfwWindowShouldClose(&w.get())) {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            shaderProgram.use();
            tri.draw();

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


