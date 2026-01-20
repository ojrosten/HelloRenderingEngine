////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/EnrichedContext/CapableContext.hpp"
#include "avocet/OpenGL/Geometry/Polygon.hpp"
#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

namespace avocet::opengl::testing {
    class pony_polygons {
        const capable_context& m_Context;

        shader_program
            m_DiscShaderProgram2D,
            m_DiscShaderProgram2DTextured,
            m_ShaderProgram2DTextured,
            m_ShaderProgram2DMonochrome,
            m_ShaderProgram2DMixedTextures,
            m_ShaderProgram3DTextured,
            m_ShaderProgram3DDoubleMonochrome;

        unique_image m_Twilight,
                     m_Fluttershy,
                     m_Hearty;

        triangle<GLfloat,   dimensionality{2}>                                                             m_Cutout;
        triangle<GLfloat,   dimensionality{2}, texture_coordinates<GLfloat>>                               m_Disc;
        quad    <GLfloat,   dimensionality{2}, texture_coordinates<GLfloat>>                               m_LowerHearts;
        quad    <GLfloat,   dimensionality{3}, texture_coordinates<GLfloat>>                               m_UpperHearts;
        quad    <GLdouble,  dimensionality{3}>                                                             m_PartiallyTransparentQuadUpper,
                                                                                                           m_PartiallyTransparentQuadLower;
        polygon<GLfloat, 6, dimensionality{2}, texture_coordinates<GLfloat>>                               m_Hexagon;
        polygon<GLfloat, 7, dimensionality{2}, texture_coordinates<GLfloat>, texture_coordinates<GLfloat>> m_Septagon;
    public:
        explicit pony_polygons(const opengl::capable_context& ctx);

        void draw();
    };
}