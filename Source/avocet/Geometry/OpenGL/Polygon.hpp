////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Buffers.hpp"
#include "avocet/Graphics/OpenGL/Textures.hpp"

#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"
#include "sequoia/PlatformSpecific/Preprocessor.hpp"

#include <cmath>
#include <limits>
#include <numbers>

#include "glad/gl.h"

namespace avocet::opengl {
    struct dimensionality{
        std::size_t value{};

        [[nodiscard]]
        constexpr friend auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, num_resources NumTextures>
        requires (3 <= N) && (dimensionality{2} <= ArenaDimension) && (ArenaDimension <= dimensionality{4})
    class polygon_base{
    public:
        using value_type = T;
        constexpr static auto num_vertices{N};
        constexpr static auto arena_dimension{ArenaDimension};
        constexpr static auto num_textures{NumTextures};
        constexpr static bool textured{num_textures.value > 0};
        constexpr static std::size_t texture_coords_per_vertex{textured ? 2 : 0};
        constexpr static auto num_coordinates{N * (arena_dimension.value + texture_coords_per_vertex)};
    private:
        struct null_texture { using configurator_type = void; };

        using texture_t = std::conditional_t<textured, texture_object<NumTextures, texture_flavour::texture_2d>, null_texture>;
        constexpr static auto dimension{arena_dimension.value};

        static void set_attribute_ptr(GLuint index, GLint components, std::ptrdiff_t offset) {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<value_type>)};
            constexpr auto stride{(dimension + texture_coords_per_vertex) * sizeof(value_type)};
            if constexpr(std::is_same_v<value_type, GLdouble>) {
                gl_function{glVertexAttribLPointer}(index, components, typeSpecifier, stride, (GLvoid*)offset);
            }
            else {
                gl_function{glVertexAttribPointer}(index, components, typeSpecifier, GL_FALSE, stride, (GLvoid*)offset);
            }
            gl_function{glEnableVertexAttribArray}(index);
        }
    public:
        using vertices_type = std::array<T, num_coordinates>;

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (!textured)
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_VAO{label}
            , m_VBO{transformer(vertices()), label}
        {
            set_attribute_ptr(0, dimension, 0);
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (textured)
        polygon_base(Fn transformer, const std::array<typename texture_t::configurator_type, NumTextures.value>& textureConfig, const std::optional<std::string>& label)
            : m_VAO{label}
            , m_VBO{transformer(vertices()), label}
            , m_Texture{textureConfig}
        {
            set_attribute_ptr(0, dimension, 0);
            set_attribute_ptr(1, 2, dimension * sizeof(value_type));
        }

        [[nodiscard]]
        friend bool operator==(const polygon_base&, const polygon_base&) noexcept = default;
    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;

        static void do_bind(const polygon_base& pg) {
            bind(pg.m_VAO);
            if constexpr(textured) {
                bind_for_rendering(pg.m_Texture);
            }
        }
    private:
        vertex_attribute_object m_VAO;
        vertex_buffer_object<value_type> m_VBO;

        SEQUOIA_NO_UNIQUE_ADDRESS texture_t m_Texture;

        [[nodiscard]]
        constexpr static T to_coordinate(std::size_t i) {
            constexpr T
                pi{std::numbers::pi_v<T>},
                offset{N % 2 ? 0 : pi / N};

            constexpr auto dim{arena_dimension.value};
            constexpr auto coordsPerVert{dim + texture_coords_per_vertex};
            const auto n{i / coordsPerVert};
            const auto theta_n{offset + 2 * pi * n / N};

            if(const auto remainder{i % coordsPerVert}; remainder == 0)
                return -T{0.5}*std::sin(theta_n);
            else if(remainder == 1)
                return T{0.5}*std::cos(theta_n);
            else if(remainder == dim)
                return T{0.5}*(T{1.0} - std::sin(theta_n));
            else if(remainder == (dim + 1))
                return T{0.5}*(T{1.0} + std::cos(theta_n));

            return T{};
        }

        [[nodiscard]]
        constexpr static vertices_type vertices() { return sequoia::utilities::make_array<T, num_coordinates>(to_coordinate); }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, num_resources NumTextures>
        requires (N <= 87)
    class polygon : public polygon_base<T, N, ArenaDimension, NumTextures> {
    public:
        using polygon_base_type = polygon_base<T, N, ArenaDimension, NumTextures>;
        using vertices_type     = polygon_base_type::vertices_type;
        constexpr static bool textured{polygon_base_type::textured};

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (!textured)
        polygon(Fn transformer, const std::optional<std::string>& label)
            : polygon_base_type{transformer, label}
            , m_EBO{st_Indices, label}
        {
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (textured)
        polygon(Fn transformer, const std::array<texture_configuration<texture_flavour::texture_2d>, NumTextures.value>& textureConfig, const std::optional<std::string>& label)
            : polygon_base_type{transformer, textureConfig, label}
            , m_EBO{st_Indices, label}
        {}

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

    template<gl_floating_point T, dimensionality ArenaDimension, num_resources NumTextures>
    class polygon<T, 3, ArenaDimension, NumTextures> : public polygon_base<T, 3, ArenaDimension, NumTextures> {
    public:
        using polygon_base_type = polygon_base<T, 3, ArenaDimension, NumTextures>;
        using polygon_base<T, 3, ArenaDimension, NumTextures>::polygon_base;

        void draw() {
            polygon_base_type::do_bind(*this);
            gl_function{glDrawArrays}(GL_TRIANGLES, 0, 3);
        }
    };

    template<gl_floating_point T, dimensionality ArenaDimension, num_resources NumTextures>
    using triangle = polygon<T, 3, ArenaDimension, NumTextures>;

    template<gl_floating_point T, dimensionality ArenaDimension, num_resources NumTextures>
    using quad = polygon<T, 4, ArenaDimension, NumTextures>;
}