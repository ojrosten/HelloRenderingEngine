////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Buffers.hpp"

namespace avocet::opengl {
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
            gl_function{glBindVertexArray}(m_VAO.get_handle().index());

            gl_function{glBindBuffer}(GL_ARRAY_BUFFER, m_VBO.get_handle().index());
            gl_function{glBufferData}(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices.data(), GL_STATIC_DRAW);

            gl_function{glVertexAttribPointer}(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            gl_function{glEnableVertexAttribArray}(0);
        }

        void draw() {
            gl_function{glBindVertexArray}(m_VAO.get_handle().index());
            gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
        }
    };
}