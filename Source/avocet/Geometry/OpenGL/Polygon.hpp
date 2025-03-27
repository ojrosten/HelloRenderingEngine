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
    template<class T, std::size_t N, class Fn>
        requires std::is_invocable_r_v<T, Fn, std::size_t>
    [[nodiscard]]
    constexpr std::array<T, N> make_array(Fn f) {
        return[&] <std::size_t... Is>(std::index_sequence<Is...>){
            return std::array<T, N>{f(Is)...};
        }(std::make_index_sequence<N>{});
    }

    struct dimensionality {
        std::size_t value{};

        [[nodiscard]]
        friend constexpr auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };


    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension>
        requires (N <= 87) && (dimensionality{2} <= ArenaDimension) && (ArenaDimension <= dimensionality{4})
    class polygon_base {
    public:
        constexpr static auto dimension{ArenaDimension};
        constexpr static std::size_t num_vertices{N};
        using value_type    = T;
        using vertices_type = std::array<T, num_vertices * dimension.value>;

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_Vertices{transformer(vertices())}
            , m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<value_type>)};
            constexpr auto dim{dimension.value};
            constexpr auto stride{dim * sizeof(value_type)};
            if constexpr(std::is_same_v<value_type, GLdouble>) {
                gl_function{glVertexAttribLPointer}(0, dim, typeSpecifier, stride, (GLvoid*)0);
            }
            else {
                gl_function{glVertexAttribPointer}(0, dim, typeSpecifier, GL_FALSE, stride, (GLvoid*)0);
            }
            gl_function{glEnableVertexAttribArray}(0);
        }
    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;

        static void do_bind(const polygon_base& pg) { bind(pg.m_VAO); }
    private:
        vertices_type m_Vertices;

        vertex_attribute_object m_VAO;
        vertex_buffer_object<value_type> m_VBO;

        constexpr static auto pi{std::numbers::pi_v<T>};

        [[nodiscard]]
        constexpr static T get_coordinate(std::size_t i) {
            constexpr auto offset{N % 2 ? 0 : pi / N};
            constexpr auto dim{dimension.value};
            const auto n{i / dim};
            const auto theta_n{offset + 2*pi*n / N};

            if(not (i % dim))
                return -T{0.5} *std::sin(theta_n);

            if(not ((i - 1) % dim))
                return T{0.5} * std::cos(theta_n);

            return T{};
        }

        [[nodiscard]]
        constexpr static vertices_type vertices() { return make_array<T, dimension.value * N>(get_coordinate); }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension>
    class polygon : public polygon_base<T, N, ArenaDimension> {
    public:
        using polygon_base_type = polygon_base<T, N, ArenaDimension>;
        using vertices_type     = polygon_base_type::vertices_type;

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon(Fn transformer, const std::optional<std::string>& label)
            : polygon_base_type{transformer, label}
            , m_EBO{st_EBOIndices, label}
        {}

        void draw() {
            polygon_base_type::do_bind(*this);
            gl_function{glDrawElements}(GL_TRIANGLES, num_element_indices, to_gl_enum(to_gl_type_specifier_v<element_index_type>), nullptr);
        }
    private:
        constexpr static std::size_t num_element_indices{3 * (N - 2)};
        using element_index_type = GLubyte;
        using element_array_type = std::array<element_index_type, num_element_indices>;

        static_assert(num_element_indices < std::numeric_limits<element_index_type>::max());

        [[nodiscard]]
        constexpr static element_index_type get_element_index(std::size_t i) noexcept {
            if(not(i % 3))
                return {};

            const auto triangleIndex{static_cast<element_index_type>(i / 3)};
            if(not((i - 1) % 3))
                return triangleIndex + 1;

            return triangleIndex + 2;
        }

        constexpr static element_array_type st_EBOIndices{make_array<element_index_type, num_element_indices>(get_element_index)};

        element_buffer_object<GLubyte> m_EBO;
    };

    template<gl_floating_point T, dimensionality ArenaDimension>
    class polygon<T, 3, ArenaDimension> : public polygon_base<T, 3, ArenaDimension> {
    public:
        using polygon_base_type = polygon_base<T, 3, ArenaDimension>;
        using polygon_base<T, 3, ArenaDimension>::polygon_base;

        void draw() {
            polygon_base_type::do_bind(*this);
            gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
        }
    };

    template<gl_floating_point T, dimensionality ArenaDimension>
    using triangle = polygon<T, 3, ArenaDimension>;

    template<gl_floating_point T, dimensionality ArenaDimension>
    using quad = polygon<T, 4, ArenaDimension>;

}