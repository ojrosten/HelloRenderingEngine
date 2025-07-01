////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Graphics/Buffers.hpp"
#include "avocet/OpenGL/Graphics/Textures.hpp"

#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"
#include "sequoia/PlatformSpecific/Preprocessor.hpp"
#include "sequoia/Maths/Geometry/Spaces.hpp"

#include <cmath>
#include <limits>
#include <numbers>

#include "glad/gl.h"

namespace avocet::opengl {
    template<std::floating_point T, std::size_t D, class Arena>
    struct euclidean_vector_space : sequoia::maths::euclidean_vector_space<T, D>
    {
    };

    struct texture_arena {};
    struct local_geometry_arena {};

    template<std::floating_point T, std::size_t D, sequoia::maths::basis Basis, class Arena>
    using euclidean_vector_coordinates = sequoia::maths::vector_coordinates<euclidean_vector_space<T, D, Arena>, Basis>;

    template<std::floating_point T, std::size_t D>
    using local_coordinates = euclidean_vector_coordinates<T, D, sequoia::maths::canonical_right_handed_basis<euclidean_vector_space<T, D, local_geometry_arena>>, local_geometry_arena>;

    template<std::floating_point T>
    using texture_coordinates = euclidean_vector_coordinates<T, 2, sequoia::maths::canonical_right_handed_basis<euclidean_vector_space<T, 2, texture_arena>>, texture_arena>;

    template<std::floating_point T, std::size_t D, class Arena>
    struct legal_buffer_type<euclidean_vector_coordinates<T, D, sequoia::maths::canonical_right_handed_basis<euclidean_vector_space<T, D, Arena>>, Arena>> 
        : std::bool_constant<sizeof(euclidean_vector_coordinates<T, D, sequoia::maths::canonical_right_handed_basis<euclidean_vector_space<T, D, Arena>>, Arena>) == D * sizeof(T)>
    {};

    struct dimensionality{
        std::size_t value{};

        [[nodiscard]]
        constexpr friend auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };

    template<gl_floating_point T, dimensionality ArenaDimension, class... Attributes>
    struct vertex_attributes {
        using value_type = T;

        local_coordinates<T, ArenaDimension.value>          local_coords;
        SEQUOIA_NO_UNIQUE_ADDRESS std::tuple<Attributes...> additional_attributes;
    };

    template<gl_floating_point T, dimensionality ArenaDimension, class... Attributes>
    struct legal_buffer_type<vertex_attributes<T, ArenaDimension, Attributes...>>
        : std::bool_constant<
                 legal_buffer_type_v<local_coordinates<T, ArenaDimension.value>>
              && (legal_buffer_type_v<Attributes> && ...)
              && (sizeof(vertex_attributes<T, ArenaDimension, Attributes...>) == (sizeof(local_coordinates<T, ArenaDimension.value>) + ... + sizeof(Attributes)))
          >
    {};

    template<gl_floating_point T, dimensionality D>
        requires (dimensionality{2} <= D)
    [[nodiscard]]
    constexpr local_coordinates<T, D.value> make_polygon_vertex(std::size_t i, std::size_t N) {
        constexpr T pi{std::numbers::pi_v<T>};
        const auto offset{N % 2 ? 0 : pi / N};
        const auto theta_n{offset + 2 * pi * i / N};

        return {-T{0.5}*std::sin(theta_n), T{0.5}*std::cos(theta_n)};
    }

    template<class>
    struct make_polygon_vertex_attribute;

    template<gl_floating_point T>
    struct make_polygon_vertex_attribute<texture_coordinates<T>> {
        [[nodiscard]]
        constexpr texture_coordinates<T> operator() (std::size_t i, std::size_t N) const {
            return texture_coordinates<T>{0.5f, 0.5f} + texture_coordinates<T>{make_polygon_vertex<T, dimensionality{2}> (i, N).values()};
        }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, class... Attributes>
    [[nodiscard]]
    constexpr vertex_attributes<T, ArenaDimension, Attributes...> make_polygon_vertex_attributes(std::size_t i) {
        return {
            .local_coords{make_polygon_vertex<T, ArenaDimension>(i, N)},
            .additional_attributes{make_polygon_vertex_attribute<Attributes>{}(i, N)...}
        };
    }

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, class... Attributes>
        requires (3 <= N) && (dimensionality{2} <= ArenaDimension) && (ArenaDimension <= dimensionality{4})
    class polygon_base{
    public:
        using value_type = T;
        constexpr static auto num_vertices{N};
        constexpr static auto arena_dimension{ArenaDimension};
        constexpr static bool is_textured_v{(std::same_as<texture_coordinates<T>, Attributes> || ...)};

        using vertex_attribute_type = vertex_attributes<T, ArenaDimension, Attributes...>;
        using vertices_type         = std::array<vertex_attribute_type, N>;

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (!is_textured_v)
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_VBO{transformer(st_Vertices), label}
            , m_VAO{label}
        {
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && is_textured_v
        polygon_base(Fn transformer, const texture_2d_configurator& texConfig, const std::optional<std::string>& label)
            : m_VBO{transformer(st_Vertices), label}
            , m_VAO{label}
            , m_Texture{texConfig}
        {
        }

        [[nodiscard]]
        friend bool operator==(const polygon_base&, const polygon_base&) noexcept = default;
    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;

        static void do_bind(const polygon_base& pg) {
            bind(pg.m_VAO);
            if constexpr(is_textured_v) {
                bind_for_rendering(pg.m_Texture);
            }
        }
    private:
        const inline static vertices_type st_Vertices{sequoia::utilities::make_array<vertex_attribute_type, N>(
            [](std::size_t i){ return make_polygon_vertex_attributes<T, N, ArenaDimension, Attributes...>(i); })};
        struct null_texture {};
        using vao_t     = vertex_attribute_object<local_coordinates<T, ArenaDimension.value>, Attributes...>;
        using texture_t = std::conditional_t<is_textured_v, texture_2d, null_texture>;

        vertex_buffer_object<vertex_attribute_type> m_VBO;
        vao_t m_VAO;
        SEQUOIA_NO_UNIQUE_ADDRESS texture_t m_Texture;
    };

    template<std::integral EBOValueType>
    constexpr std::size_t max_num_supported_vertices() {
        return 2 + (std::numeric_limits<EBOValueType>::max() / 3);
    }

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, class... Attributes>
        requires (N <= max_num_supported_vertices<GLubyte>())
    class polygon : public polygon_base<T, N, ArenaDimension, Attributes...> {
    public:
        using polygon_base_type = polygon_base<T, N, ArenaDimension, Attributes...>;
        using vertices_type     = polygon_base_type::vertices_type;
        constexpr static bool is_textured_v{polygon_base_type::is_textured_v};

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (!is_textured_v)
        polygon(Fn transformer, const std::optional<std::string>& label)
            : polygon_base_type{transformer, label}
            , m_EBO{st_Indices, label}
        {
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && is_textured_v
        polygon(Fn transformer, const texture_2d_configurator& texConfig, const std::optional<std::string>& label)
            : polygon_base_type{transformer, texConfig, label}
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

    template<gl_floating_point T, dimensionality ArenaDimension, class... Attributes>
    class polygon<T, 3, ArenaDimension, Attributes...> : public polygon_base<T, 3, ArenaDimension, Attributes...> {
    public:
        using polygon_base_type = polygon_base<T, 3, ArenaDimension, Attributes...>;
        using polygon_base<T, 3, ArenaDimension, Attributes...>::polygon_base;

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
