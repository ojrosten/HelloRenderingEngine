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

    struct dimensionality {
        std::size_t value{};

        [[nodiscard]]
        friend auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };

    template<class G>
    concept geometry_specification = requires{
        typename G::value_type;
        requires gl_arithmetic_type<typename G::value_type>;
        { G::dimension    } -> std::convertible_to<dimensionality>;
        { G::num_vertices } -> std::convertible_to<std::size_t>;
        { G::vertices }     -> std::convertible_to<std::array<typename G::value_type, G::num_elements>>;
    };

    template<geometry_specification G>
    using vertices_type = std::remove_const_t<decltype(G::vertices)>;

    template<gl_arithmetic_type T, std::size_t N>
    using polygon_vertices_type = std::array<T, N>;

    template<std::size_t NumVertices>
    struct polygon_specification_base{
        constexpr static dimensionality dimension{2};
        constexpr static auto num_vertices{NumVertices};
        constexpr static auto num_elements{num_vertices * dimension.value};
    };

    template<gl_arithmetic_type T>
    struct triangle_specification : polygon_specification_base<3> {
        using value_type = T;

        constexpr static polygon_vertices_type<T, num_elements> vertices{
           -0.5f, -0.5f, // left  
            0.5f, -0.5f, // right 
            0.0f,  0.5f  // top   
        };
    };

    template<gl_arithmetic_type T>
    struct quad_specification : polygon_specification_base<4> {
        using value_type = T;

        constexpr static polygon_vertices_type<T, num_elements> vertices{
            -0.5f, -0.5f,
             0.5f, -0.5f,
             0.5f,  0.5f,
            -0.5f,  0.5f
        };
    };

    template<dimensionality To, geometry_specification G>
        requires (To == G::dimension)
    [[nodiscard]]
    constexpr const vertices_type<G>& embed_poly(const vertices_type<G>& in) noexcept{
        return in;
    }


    template<dimensionality To, geometry_specification G>
        requires (To > G::dimension)
    [[nodiscard]]
    constexpr polygon_vertices_type<typename G::value_type, To.value* G::num_vertices> embed_poly(const vertices_type<G>& in) noexcept {
        using T = typename G::value_type;
        constexpr auto embeddingDim{To.value};
        constexpr auto intrinsicDim{G::dimension.value};
        constexpr auto numOutputElements{embeddingDim * G::num_vertices};
        using array_t = polygon_vertices_type<T, numOutputElements>;

        return [&] <std::size_t... Is>(std::index_sequence<Is...>) {
            return array_t{(((Is + 1) % embeddingDim) ? in[(Is / embeddingDim) * intrinsicDim + (Is % embeddingDim)] : T{})...};
        }(std::make_index_sequence<numOutputElements>{});
    }

    template<geometry_specification G, dimensionality EmbeddingSpaceDimension>
      requires (EmbeddingSpaceDimension >= G::dimension)
    class primitive_geometry {
    public:
        using value_type = G::value_type;
        constexpr static auto embedding_dimension{EmbeddingSpaceDimension};

        template<class Fn = std::identity>
            requires std::is_invocable_r_v<vertices_type<G>, Fn, vertices_type<G>>
        explicit primitive_geometry(const std::optional<std::string>& label, Fn transformer = {})
            : m_Vertices{embed_poly<embedding_dimension, G>(transformer(G::vertices))}
            , m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            const auto dim{static_cast<GLint>(embedding_dimension.value)};
            gl_function{glVertexAttribPointer}(0, dim, to_gl_enum(to_gl_type_constant_v<value_type>), GL_FALSE, dim * sizeof(value_type), (GLvoid*)0);
            gl_function{glEnableVertexAttribArray}(0);
        }

        friend void bind(const primitive_geometry& pg) { bind(pg.m_VAO); }
    private:
        using embedded_vertices_type = polygon_vertices_type<value_type, G::num_vertices* embedding_dimension.value>;
        embedded_vertices_type m_Vertices;

        vertex_attribute_object m_VAO;
        vertex_buffer_object<GLfloat> m_VBO;
    };

    template<dimensionality EmbeddingSpaceDimension = dimensionality{3}>
    class triangle {
        primitive_geometry<triangle_specification<GLfloat>, EmbeddingSpaceDimension> m_Geom;
    public:
        template<class Fn=std::identity>
        explicit triangle(const std::optional<std::string>& label, Fn transformer = {})
            : m_Geom{label, transformer}
        {}

        void draw() {
            bind(m_Geom);
            gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
        }
    };

    template<dimensionality EmbeddingSpaceDimension = dimensionality{3}>
    class quad {
        primitive_geometry<quad_specification<GLfloat>, EmbeddingSpaceDimension> m_Geom;

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

        void draw() {
            bind(m_Geom);
            gl_function{glDrawElements}(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        }
    };
}