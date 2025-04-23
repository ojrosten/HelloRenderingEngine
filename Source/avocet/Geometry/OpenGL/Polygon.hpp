////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Resources.hpp"
#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"

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

    struct texture_count{
        std::size_t value{};

        [[nodiscard]]
        constexpr friend auto operator<=>(const texture_count&, const texture_count&) noexcept = default;
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, texture_count NumTextures>
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

        using vertices_type = std::array<T, num_coordinates>;

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (!textured)
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_VAO{label}
            , m_VBO{transformer(vertices()), label}
        {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<value_type>)};
            constexpr auto dimension{arena_dimension.value};
            constexpr auto stride{(dimension + texture_coords_per_vertex) * sizeof(value_type)};
            if constexpr(std::is_same_v<value_type, GLdouble>) {
                gl_function{glVertexAttribLPointer}(0, dimension, typeSpecifier, stride, (GLvoid*)0);
            }
            else {
                gl_function{glVertexAttribPointer}(0, dimension, typeSpecifier, GL_FALSE, stride, (GLvoid*)0);
            }
            gl_function{glEnableVertexAttribArray}(0);

            if constexpr(textured) {
                if constexpr(std::is_same_v<value_type, GLdouble>) {
                    gl_function{glVertexAttribLPointer}(1, 2, typeSpecifier, stride, (GLvoid*)(dimension * sizeof(value_type)));
                }
                else {
                    gl_function{glVertexAttribPointer}(1, 2, typeSpecifier, GL_FALSE, stride, (GLvoid*)(dimension * sizeof(value_type)));
                }
            }

            gl_function{glEnableVertexAttribArray}(1);
        }

        /*template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (textured)
        polygon_base(Fn transformer, std::span<const std:filesystem::path, num_textures.value>, const std::optional<std::string>& label)
            :
        {
        }*/

        [[nodiscard]]
        friend bool operator==(const polygon_base& lhs, const polygon_base& rhs) noexcept = default;
    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;

        static void do_bind(const polygon_base& pg) { bind(pg.m_VAO); }
    private:
        vertex_attribute_object m_VAO;
        vertex_buffer_object<value_type> m_VBO;
        //std::array<texture_object, num_textures.value> m_Textures;

        [[nodiscard]]
        constexpr static T to_coordinate(std::size_t i) {
            constexpr T
                pi{std::numbers::pi_v<T>},
                offset{N % 2 ? 0 : pi / N};

            constexpr auto dim{arena_dimension.value};
            const auto n{i / dim};
            const auto theta_n{offset + 2 * pi * n / N};

            if(const auto remainder{i % (dim + texture_coords_per_vertex)}; remainder == 0)
                return -T{0.5}*std::sin(theta_n);
            else if(remainder == 1)
                return T{0.5}*std::cos(theta_n);
            else if(remainder == dim)
                return T{0.5}*(T{1.0} - std::sin(theta_n));
            else if(remainder == (dim+1))
                return T{0.5}*(T{1.0} + std::cos(theta_n));

            return T{};
        }

        [[nodiscard]]
        constexpr static vertices_type vertices() { return sequoia::utilities::make_array<T, num_coordinates>(to_coordinate); }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension>
        requires (N <= 87)
    class polygon : public polygon_base<T, N, ArenaDimension, texture_count{0}> {
    public:
        using polygon_base_type = polygon_base<T, N, ArenaDimension, texture_count{0}>;
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
    class polygon<T, 3, ArenaDimension> : public polygon_base<T, 3, ArenaDimension, texture_count{0} > {
    public:
        using polygon_base_type = polygon_base<T, 3, ArenaDimension, texture_count{0}>;
        using polygon_base<T, 3, ArenaDimension, texture_count{0}>::polygon_base;

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