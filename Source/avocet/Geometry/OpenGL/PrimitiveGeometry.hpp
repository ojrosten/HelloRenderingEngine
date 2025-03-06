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
    enum class gl_type_constant : GLenum {
        gl_float = GL_FLOAT
    };

    template<gl_arithmetic_type>
    struct to_gl_type_constant;

    template<gl_arithmetic_type G>
    inline constexpr gl_type_constant to_gl_type_constant_v{to_gl_type_constant<G>::value};

    template<>
    struct to_gl_type_constant<GLfloat> {
        constexpr static auto value{gl_type_constant::gl_float};
    };

    template<class G>
    concept geometry_specification = requires{
        typename G::value_type;
        requires gl_arithmetic_type<typename G::value_type>;
        { G::num_vertices } -> std::convertible_to<std::size_t>;
        { G::dimension    } -> std::convertible_to<std::size_t>;
        { G::vertices }     -> std::convertible_to<std::array<typename G::value_type, G::num_elements>>;
    };

    template<geometry_specification G>
    using vertices_type = std::remove_const_t<decltype(G::vertices)>;

    template<gl_arithmetic_type T>
    struct triangle_specification {
        using value_type = T;
        constexpr static std::size_t dimension{3};
        constexpr static std::size_t num_vertices{3};
        constexpr static auto num_elements{num_vertices * dimension};

        constexpr static std::array<T, num_elements> vertices{
           -0.5f, -0.5f, 0.0f, // left  
            0.5f, -0.5f, 0.0f, // right 
            0.0f,  0.5f, 0.0f  // top   
        };
    };

    template<gl_arithmetic_type T>
    struct quad_specification {
        using value_type = T;
        constexpr static std::size_t dimension{3};
        constexpr static std::size_t num_vertices{4};
        constexpr static auto num_elements{num_vertices * dimension};

        constexpr static std::array<T, num_elements> vertices{
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f
        };
    };

    template<geometry_specification G>
    class primitive_geometry {
    public:
        using value_type = G::value_type;
        constexpr static auto dimension{G::dimension};

        template<class Fn = std::identity>
            requires std::is_invocable_r_v<vertices_type<G>, Fn, vertices_type<G>>
        explicit primitive_geometry(const std::optional<std::string>& label, Fn transformer = {})
            : m_Vertices{transformer(G::vertices)}
            , m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            gl_function{glVertexAttribPointer}(0, dimension, to_gl_enum(to_gl_type_constant_v<value_type>), GL_FALSE, dimension * sizeof(value_type), (GLvoid*)0);
            gl_function{glEnableVertexAttribArray}(0);
        }

        friend void bind(const primitive_geometry& pg) { bind(pg.m_VAO); }
    private:
        vertices_type<G> m_Vertices;

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