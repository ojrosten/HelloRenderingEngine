////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Buffers.hpp"

#include "glad/gl.h"

namespace avocet::opengl {
    class triangle {
    public:
        explicit triangle(const std::optional<std::string>& label)
            : m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            gl_function{glVertexAttribPointer}(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            gl_function{glEnableVertexAttribArray}(0);
        }

        void draw() {
            bind(m_VAO);
            gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
        }
    private:
        std::array<GLfloat, 9> m_Vertices{
            -0.5, -0.5, 0.0, // left  
             0.5, -0.5, 0.0, // right 
             0.0,  0.5, 0.0  // top   
        };

        vertex_attribute_object m_VAO;
        vertex_buffer_object<GLfloat> m_VBO;
    };

    class quad {
    public:
        explicit quad(const std::optional<std::string>& label)
            : m_VAO{label}
            , m_VBO{m_Vertices, label}
            , m_EBO{m_Indices, label}
        {
            gl_function{glVertexAttribPointer}(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            gl_function{glEnableVertexAttribArray}(0);
        }

        void draw() {
            bind(m_VAO);
            gl_function{glDrawElements}(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        }
    private:
        std::array<GLfloat, 12> m_Vertices{
            -0.5, -0.5, 0.0,
             0.5, -0.5, 0.0,
             0.5,  0.5, 0.0,
            -0.5,  0.5, 0.0
        };

        std::array<GLubyte, 6> m_Indices{
            0, 1, 2,
            0, 2, 3
        };

        vertex_attribute_object m_VAO;
        vertex_buffer_object<GLfloat> m_VBO;
        element_buffer_object<GLubyte> m_EBO;
    };
}