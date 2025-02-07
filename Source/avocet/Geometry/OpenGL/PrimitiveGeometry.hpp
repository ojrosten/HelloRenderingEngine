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
    template<gl_arithmetic_type T>
    struct triangle_specification {
        using value_type = T;
        constexpr static auto num_vertices{3};
        constexpr static auto num_elements{num_vertices * 3};

        constexpr static std::array<T, num_elements> vertices{
           -0.5f, -0.5f, 0.0f, // left  
            0.5f, -0.5f, 0.0f, // right 
            0.0f,  0.5f, 0.0f  // top   
        };
    };

    template<gl_arithmetic_type T>
    struct quad_specification {
        using value_type = T;
        constexpr static auto num_vertices{4};
        constexpr static auto num_elements{num_vertices * 3};

        constexpr static std::array<GLfloat, num_elements> vertices{
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };
    };

    template<class G>
    concept geometry_specification = requires{
        typename G::value_type;
        { G::num_vertices } -> std::convertible_to<std::size_t>;
        { G::num_elements } -> std::convertible_to<std::size_t>;
        { G::vertices }     -> std::convertible_to<std::array<typename G::value_type, G::num_elements>>;
    };

    template<geometry_specification Specification>
    class primitive_geometry {
    public:
        using value_type = Specification::value_type;
        constexpr static auto num_vertices{Specification::num_elements};

        template<class Fn = std::identity>
        explicit primitive_geometry(const std::optional<std::string>& label, Fn transformer = {})
            : m_Vertices{transformer(Specification::vertices)}
            , m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            gl_function{glVertexAttribPointer}(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
            gl_function{glEnableVertexAttribArray}(0);
        }

        friend void bind(const primitive_geometry& pg) { bind(pg.m_VAO); }
    private:
        std::array<value_type, num_vertices> m_Vertices;

        vertex_attribute_object m_VAO;
        vertex_buffer_object<GLfloat> m_VBO;
    };

    class triangle {
        primitive_geometry<triangle_specification<GLfloat>> m_Geom;
    public:
        template<class Fn=std::identity>
        explicit triangle(const std::optional<std::string>& label, Fn transformer = {})
            : m_Geom{label, transformer}
        {}

        void draw();
    };

    class quad {
        primitive_geometry<quad_specification<GLfloat>> m_Geom;

        std::array<GLubyte, 6> m_Indices{
            0, 1, 2,
            0, 2, 3
        };

        element_buffer_object<GLubyte> m_EBO;
    public:
        template<class Fn = std::identity>
        explicit quad(const std::optional<std::string>& label, Fn transformer = {})
            : m_Geom{label, transformer}
            , m_EBO{m_Indices, label}
        {}

        void draw();
    };
}