////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Geometry/OpenGL/PrimitiveGeometry.hpp"

namespace avocet::opengl {
    void triangle::draw() {
        bind(m_Geom);
        gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
    }

    void quad::draw() {
        bind(m_Geom);
        gl_function{glDrawElements}(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
    }
}