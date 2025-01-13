////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Geometry/OpenGL/PrimitiveGeometry.hpp"

namespace avocet::opengl {
    triangle::triangle(const std::optional<std::string>& label)
        : m_VAO{label}
        , m_VBO{label}
    {
        gl_function{glBufferData}(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices.data(), GL_STATIC_DRAW);
    
        gl_function{glVertexAttribPointer}(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        gl_function{glEnableVertexAttribArray}(0);
    }
    
    void triangle::draw() {
        gl_function{glBindVertexArray}(get_raw_index(m_VAO));
        gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
    }

    quad::quad(const std::optional<std::string>& label)
        : m_VAO{label}
        , m_VBO{label}
        , m_EBO{label}
    {
        gl_function{glBufferData}(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices.data(), GL_STATIC_DRAW);
        gl_function{glBufferData}(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), m_Indices.data(), GL_STATIC_DRAW);

        gl_function{glVertexAttribPointer}(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
        gl_function{glEnableVertexAttribArray}(0);
    }

    void quad::draw() {
        m_VAO.bind();
        gl_function{glDrawElements}(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
    }
}