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
        std::array<GLfloat, 9> m_Vertices{
            -0.5f, -0.5f, 0.0f, // left  
             0.5f, -0.5f, 0.0f, // right 
             0.0f,  0.5f, 0.0f  // top   
        };

        vertex_attribute_object m_VAO;
        vertex_buffer_object<GLfloat> m_VBO;
    public:
        explicit triangle(const std::optional<std::string>& label);

        void draw();
    };

    class quad {
        std::array<GLfloat, 12> m_Vertices{
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };

        std::array<GLubyte, 6> m_Indices{
            0, 1, 2,
            0, 2, 3
        };

        vertex_attribute_object m_VAO;
        vertex_buffer_object<GLfloat> m_VBO;
        element_buffer_object<GLubyte> m_EBO;
    public:
        explicit quad(const std::optional<std::string>& label);

        void draw();
    };
}