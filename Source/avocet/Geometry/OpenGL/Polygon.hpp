////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Buffers.hpp"
#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"

#include <cmath>
#include <limits>
#include <numbers>

#include "glad/gl.h"

namespace avocet::opengl {

    template<class G>
    concept geometry_specification = requires {
        typename G::value_type;
        requires gl_floating_point<typename G::value_type>;

        { G::dimension } -> std::convertible_to<std::size_t>;
        requires (0 < G::dimension) && (G::dimension <= 4);
        
        typename G::vertices_type;
        { G::vertices } -> std::convertible_to<typename G::vertices_type>;
    };

    template<gl_floating_point T, std::size_t NumVertices>
    struct geometry_specification_base {
        using value_type = T;
        constexpr static std::size_t
            num_vertices{NumVertices},
            dimension{3},
            num_elements{dimension * num_vertices};

        using vertices_type = std::array<T, num_elements>;
    };

    template<gl_floating_point T>
    struct triangle_specification : geometry_specification_base<T, 3> {
        using vertices_type = geometry_specification_base<T, 3>::vertices_type;

        constexpr static vertices_type vertices{
            -0.5, -0.5, 0.0, // left  
             0.5, -0.5, 0.0, // right 
             0.0,  0.5, 0.0  // top   
        };
    };

    template<gl_floating_point T>
    struct quad_specification : geometry_specification_base<T, 4> {
        using vertices_type = geometry_specification_base<T, 4>::vertices_type;

        constexpr static vertices_type vertices{
            -0.5, -0.5, 0.0,
             0.5, -0.5, 0.0,
             0.5,  0.5, 0.0,
            -0.5,  0.5, 0.0
        };
    };

    template<geometry_specification G>
    class primitive_geometry{
    public:
        using geometry_specification_type = G;
        using value_type                  = G::value_type;
        using vertices_type               = G::vertices_type;
        constexpr static std::size_t dimension{G::dimension};

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        primitive_geometry(Fn transformer, const std::optional<std::string>& label)
            : m_Vertices{transformer(G::vertices)}
            , m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<value_type>)};
            constexpr auto stride{dimension * sizeof(value_type)};
            if constexpr(std::is_same_v<value_type, GLdouble>) {
                gl_function{glVertexAttribLPointer}(0, dimension, typeSpecifier, stride, (GLvoid*)0);
            }
            else {
                gl_function{glVertexAttribPointer}(0, dimension, typeSpecifier, GL_FALSE, stride, (GLvoid*)0);
            }
            gl_function{glEnableVertexAttribArray}(0);
        }

        friend void bind(const primitive_geometry& pg) { bind(pg.m_VAO); }
    private:
        vertices_type m_Vertices;

        vertex_attribute_object m_VAO;
        vertex_buffer_object<value_type> m_VBO;
    };

    template<gl_floating_point T>
    class quad {
    public:
        using value_type              = T;
        using primitive_geometry_type = primitive_geometry<quad_specification<T>>;
        using vertices_type           = primitive_geometry_type::vertices_type;

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        quad(Fn transformer, const std::optional<std::string>& label)
            : m_Geometry{transformer, label}
            , m_EBO{m_Indices, label}
        {}

        void draw() {
            bind(m_Geometry);
            gl_function{glDrawElements}(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        }
    private:
        primitive_geometry_type m_Geometry;

        std::array<GLubyte, 6> m_Indices{
            0, 1, 2,
            0, 2, 3
        };

        element_buffer_object<GLubyte> m_EBO;
    };

    template<gl_floating_point T>
    class triangle {
    public:
        using value_type = T;
        using primitive_geometry_type = primitive_geometry<triangle_specification<T>>;
        using vertices_type = primitive_geometry_type::vertices_type;

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        triangle(Fn transformer, const std::optional<std::string>& label)
            : m_Geometry{transformer, label}
        {
        }

        void draw() {
            bind(m_Geometry);
            gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
        }
    private:
        primitive_geometry_type m_Geometry;
    };
}