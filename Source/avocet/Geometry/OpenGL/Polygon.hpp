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
    struct dimensionality {
        std::size_t value{};

        [[nodiscard]]
        friend constexpr auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension>
        requires (3 <= N) && (dimensionality{2} <= ArenaDimension) && (ArenaDimension <= dimensionality{4})
    class polygon_base{
    public:
        using value_type = T;
        constexpr static auto num_vertices{N};
        constexpr static auto arena_dimension{ArenaDimension};
        constexpr static auto num_coordinates{N * arena_dimension.value};

        using vertices_type = std::array<T, num_coordinates>;

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_Vertices{transformer(vertices())}
            , m_VAO{label}
            , m_VBO{m_Vertices, label}
        {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<value_type>)};
            constexpr auto dimension{arena_dimension.value};
            constexpr auto stride{dimension * sizeof(value_type)};
            if constexpr(std::is_same_v<value_type, GLdouble>) {
                gl_function{glVertexAttribLPointer}(0, dimension, typeSpecifier, stride, (GLvoid*)0);
            }
            else {
                gl_function{glVertexAttribPointer}(0, dimension, typeSpecifier, GL_FALSE, stride, (GLvoid*)0);
            }
            gl_function{glEnableVertexAttribArray}(0);
        }

        [[nodiscard]]
        friend bool operator==(const polygon_base&, const polygon_base&) noexcept = default;
    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;

        static void do_bind(const polygon_base& pg) { bind(pg.m_VAO); }
    private:
        vertices_type m_Vertices;

        vertex_attribute_object m_VAO;
        vertex_buffer_object<value_type> m_VBO;

        [[nodiscard]]
        constexpr static T to_coordinate(std::size_t i) {
            constexpr auto
                pi{std::numbers::pi_v<T>},
                offset{N % 2 ? 0 : pi / N};

            constexpr auto dim{arena_dimension.value};
            const auto n{i / dim};
            const auto theta_n{offset + 2 * pi * n / N};

            if(const auto remainder{i % dim}; remainder == 0)
                return -T{0.5}*std::sin(theta_n);
            else if(remainder == 1)
                return T{0.5} *std::cos(theta_n);

            return T{};
        }

        [[nodiscard]]
        constexpr static vertices_type vertices() { return sequoia::utilities::make_array<T, num_coordinates>(to_coordinate); }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension>
        requires (N<=87)
    class polygon : public polygon_base<T, N, ArenaDimension> {
    public:
        using polygon_base_type = polygon_base<T, N, ArenaDimension>;
        using vertices_type     = polygon_base_type::vertices_type;

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon(Fn transformer, const std::optional<std::string>& label)
            : polygon_base_type{transformer, label}
            , m_EBO{st_Indices, label}
        {
        }

        void draw() {
            polygon_base_type::do_bind(*this);
            gl_function{glDrawElements}(GL_TRIANGLES, num_element_indices, to_gl_enum(to_gl_type_specifier_v<element_index_type>), nullptr);
        }
    private:
        using element_index_type = GLubyte;
        constexpr static auto num_element_indices{3 * (N - 2)};
        using element_array_type = std::array<element_index_type, num_element_indices>;

        static_assert(num_element_indices <= std::numeric_limits<element_index_type>::max());

        [[nodiscard]]
        constexpr static element_index_type to_element_index(std::size_t i) noexcept {
            const auto remainder{i % 3};
            if(!remainder) return 0;

            return static_cast<element_index_type>(i / 3 + remainder);
        }

        constexpr static element_array_type st_Indices{
            sequoia::utilities::make_array<element_index_type, num_element_indices>(to_element_index)
        };

        element_buffer_object<element_index_type> m_EBO;
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