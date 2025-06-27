////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Graphics/Buffers.hpp"
#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"

#include "sequoia/PlatformSpecific/Preprocessor.hpp"
#include "sequoia/Maths/Geometry/Spaces.hpp"

#include <cmath>
#include <limits>
#include <numbers>

#include "glad/gl.h"

namespace avocet::opengl {
    template<std::floating_point T, std::size_t D, class Arena>
    struct euclidean_vector_space
    {
        using set_type        = sequoia::maths::sets::R<D>;
        using field_type      = T;
        using is_vector_space = std::true_type;
        constexpr static std::size_t dimension{D};
    };

    template<std::floating_point T, std::size_t D, class Arena>
    struct euclidean_affine_space
    {
        using set_type          = sequoia::maths::sets::R<D>;
        using vector_space_type = euclidean_vector_space<T, D, Arena>;
        using is_affine_space   = std::true_type;
    };

    struct texture_arena {};
    struct geometry_arena{};
    struct local_origin{};

    template<std::floating_point T, std::size_t D, class Arena>
    struct standard_basis
    {
        using vector_space_type = euclidean_vector_space<T, D, Arena>;
        using orthonormal       = std::true_type;
    };

    template<std::floating_point T, std::size_t D, sequoia::maths::basis Basis, class Arena, class Origin>
    using euclidean_affine_coordinates = sequoia::maths::affine_coordinates<euclidean_affine_space<T, D, Arena>, Basis, Origin>;

    template<std::floating_point T, std::size_t D>
    using local_position = euclidean_affine_coordinates<T, D, standard_basis<T, D, geometry_arena>, geometry_arena, local_origin>;



    struct dimensionality{
        std::size_t value{};

        [[nodiscard]]
        constexpr friend auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };

    template<dimensionality ArenaDimension>
    inline constexpr bool is_legal_dimension_v{(dimensionality{2} <= ArenaDimension) && (ArenaDimension <= dimensionality{4})};

    template<gl_floating_point T, dimensionality ArenaDimension, class... Attributes>
    struct vertex_attributes {
        using value_type = T;

        local_position<T, ArenaDimension.value>             local_coordinates;
        SEQUOIA_NO_UNIQUE_ADDRESS std::tuple<Attributes...> additional_attributes;
    };

    template<class>
    struct build_vertex_attribute;


    template<gl_floating_point T, std::size_t D>
    struct build_vertex_attribute<local_position<T, D>> {
        [[nodiscard]]
        constexpr std::array<T, D> operator() (std::size_t i, std::size_t N) const {
            constexpr T pi{std::numbers::pi_v<T>};
            const auto offset{N % 2 ? 0 : pi / N};
            const auto theta_n{offset + 2 * pi * i / N};

            return {-T{0.5}*std::sin(theta_n), T{0.5}*std::cos(theta_n)};
        }
    };


    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, class... Attributes>
        requires (3 <= N) && is_legal_dimension_v<ArenaDimension>
    class polygon_base{
    public:
        using value_type = T;
        constexpr static auto num_vertices{N};
        constexpr static auto arena_dimension{ArenaDimension};

        using vertex_attribute_type = vertex_attributes<T, ArenaDimension, Attributes...>;
        using vertices_type         = std::array<vertex_attribute_type, N>;

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_VAO{label}
            , m_VBO{transformer(st_Vertices), label}
        {
            auto nextParams{set_attribute_ptr({.index{0}, .offset{0}}, to_gl_int(arena_dimension.value))};
            ((nextParams = set_attribute_ptr(nextParams, sizeof(Attributes) / sizeof(value_type))), ...);
        }

        [[nodiscard]]
        friend bool operator==(const polygon_base&, const polygon_base&) noexcept = default;
    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;

        static void do_bind(const polygon_base& pg) { bind(pg.m_VAO); }
    private:
        [[nodiscard]]
        constexpr static vertex_attribute_type to_vertex_attributes(std::size_t i) {
            return {.local_coordinates{build_vertex_attribute<local_position<T, ArenaDimension.value>>{}(i, N)}, .additional_attributes{build_vertex_attribute<Attributes>{}(i, N)...}};
        }

        const inline static vertices_type st_Vertices{sequoia::utilities::make_array<vertex_attribute_type, N>(to_vertex_attributes)};

        vertex_attribute_object m_VAO;
        vertex_buffer_object<vertex_attribute_type> m_VBO;

        struct next_attribute_indices {
            GLuint index{};
            std::size_t offset{};
        };

        [[nodiscard]]
        static next_attribute_indices set_attribute_ptr(next_attribute_indices indices, GLint components) {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<value_type>)};
            constexpr auto stride{arena_dimension.value * sizeof(value_type) + (0 + ... + sizeof(Attributes))};
            if constexpr(std::is_same_v<value_type, GLdouble>) {
                gl_function{glVertexAttribLPointer}(indices.index, components, typeSpecifier, stride, (GLvoid*)indices.offset);
            }
            else {
                gl_function{glVertexAttribPointer}(indices.index, components, typeSpecifier, GL_FALSE, stride, (GLvoid*)indices.offset);
            }
            gl_function{glEnableVertexAttribArray}(indices.index);

            return {.index{indices.index + 1}, .offset{indices.offset + components * sizeof(value_type)}};
        }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension>
        requires (N <= 87)
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
            gl_function{glDrawElements}(GL_TRIANGLES, num_elements, to_gl_enum(to_gl_type_specifier_v<element_index_type>), nullptr);
        }
    private:
        using element_index_type = GLubyte;
        constexpr static auto num_elements{3 * (N - 2)};
        using element_array_type = std::array<element_index_type, num_elements>;

        static_assert(num_elements <= std::numeric_limits<element_index_type>::max());

        [[nodiscard]]
        constexpr static element_index_type to_element_index(std::size_t i) noexcept {
            const auto remainder{i % 3};
            if(!remainder)
                return 0;

            return static_cast<element_index_type>(i / 3 + remainder);
        }

        constexpr static element_array_type st_Indices{
            sequoia::utilities::make_array<element_index_type, num_elements>(to_element_index)
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