////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Buffers.hpp"

#include <cmath>
#include <numbers>

#include "glad/gl.h"

namespace avocet::opengl {
    template<gl_floating_point T, std::size_t N, std::size_t EmbeddingDimension>
       requires (2 <= EmbeddingDimension) && (EmbeddingDimension <= 4)
    class polygon_base{
    public:
        constexpr static std::size_t
            embedding_dimension{EmbeddingDimension},
            num_vertices{N},
            num_elements{embedding_dimension * N};

        using value_type    = T;
        using vertices_type = std::array<T, embedding_dimension * N>;

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_Vertices{transformer(vertices())}
            , m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<value_type>)};
            constexpr auto stride{embedding_dimension * sizeof(value_type)};
            if constexpr(std::is_same_v<value_type, GLdouble>) {
                gl_function{glVertexAttribLPointer}(0, embedding_dimension, typeSpecifier, stride, (GLvoid*)0);
            }
            else {
                gl_function{glVertexAttribPointer}(0, embedding_dimension, typeSpecifier, GL_FALSE, stride, (GLvoid*)0);
            }
            gl_function{glEnableVertexAttribArray}(0);
        }

    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;

        static void do_bind(const polygon_base& pb) { bind(pb.m_VAO); }
    private:
        vertices_type m_Vertices;

        vertex_attribute_object m_VAO;
        vertex_buffer_object<value_type> m_VBO;

        constexpr static T pi{std::numbers::pi_v<T>};

        template<std::size_t I>
        constexpr static T to_element() {
            if constexpr(not (I % embedding_dimension))
                return T{0.5} *std::sin(2 * pi * (I / embedding_dimension) / N);
            else if constexpr(not ((I - 1) % embedding_dimension))
                return T{0.5} * std::cos(2 * pi * (I/ embedding_dimension) / N);
            else
                return T{};
        }

        [[nodiscard]]
        constexpr static vertices_type vertices() {
            return [] <std::size_t... Is>(std::index_sequence<Is...>){
                return vertices_type{to_element<Is>()...};
            }(std::make_index_sequence<num_elements>{});
        }
    };

    template<gl_floating_point T, std::size_t EmbeddingDimension>
    class triangle : polygon_base<T, 3, EmbeddingDimension> {
    public:
        using polygon_base_type = polygon_base<T, 3, EmbeddingDimension>;
        using polygon_base<T, 3, EmbeddingDimension>::polygon_base;

        void draw() {
            polygon_base_type::do_bind(*this);
            gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
        }
    };

    template<gl_floating_point T, std::size_t EmbeddingDimension>
    class quad : polygon_base<T, 4, EmbeddingDimension> {
    public:
        using polygon_base_type = polygon_base<T, 4, EmbeddingDimension>;
        using vertices_type     = polygon_base_type::vertices_type;

        constexpr static std::size_t num_vertices{4};

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        quad(Fn transformer, const std::optional<std::string>& label)
            : polygon_base<T, 4, EmbeddingDimension>{transformer, label}
            , m_EBO{st_ElementIndices, label}
        {}

        void draw() {
            polygon_base_type::do_bind(*this);
            gl_function{glDrawElements}(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        }
    private:
        template<std::size_t I>
        [[nodiscard]]
        constexpr static GLubyte to_element_index() {
            if constexpr(not (I % 3))
                return 0;
            else if constexpr(not ((I - 1) % 3))
                return 1 + (I / 3);
            else
                return 2 + (I / 3);
        }

        constexpr static std::size_t num_element_indices{(num_vertices - 2) * 3};

        using index_array_type = std::array<GLubyte, num_element_indices>;

        [[nodiscard]]
        constexpr static index_array_type make_indices() noexcept {
            return [] <std::size_t... Is>(std::index_sequence<Is...>){
                return index_array_type{to_element_index<Is>()...};
            }(std::make_index_sequence<num_element_indices>{});
        }

        constexpr static index_array_type st_ElementIndices{make_indices()};

        element_buffer_object<GLubyte> m_EBO;
    };
}