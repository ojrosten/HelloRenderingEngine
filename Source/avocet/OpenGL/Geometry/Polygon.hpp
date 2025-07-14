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
#include "sequoia/Maths/Geometry/Spaces.hpp"
#include "sequoia/PlatformSpecific/Preprocessor.hpp"

#include <cmath>
#include <limits>
#include <numbers>

namespace avocet::opengl {
    struct dimensionality{
        std::size_t value{};

        [[nodiscard]]
        constexpr friend auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };

    struct local_geometry_arena {};

    struct texture_arena {};

    template<std::floating_point T, dimensionality D>
    using local_coordinates   = sequoia::maths::vec_coords<T, D.value, local_geometry_arena>;

    template<std::floating_point T>
    using texture_coordinates = sequoia::maths::vec_coords<T, 2, texture_arena>;

    template<std::floating_point T, dimensionality D>
    [[nodiscard]]
    constexpr local_coordinates<T, D> make_polygon_coordinates(std::size_t i, std::size_t N) {
        constexpr T pi{std::numbers::pi_v<T>};
        const auto offset{N % 2 ? 0 : pi / N};

        const auto theta_n{offset + 2 * pi * i / N};

        return {-T{0.5}*std::sin(theta_n), T{0.5}*std::cos(theta_n)};
    }

    template<std::floating_point T>
    [[nodiscard]]
    constexpr texture_coordinates<T> make_polygon_tex_coordinates(std::size_t i, std::size_t N) {
        return texture_coordinates<T>{T{0.5}, T{0.5}} + texture_coordinates<T>{make_polygon_coordinates<T, dimensionality{2}>(i, N).values()};
    }

    template<class T>
    struct make_polygon_attribute;

    template<std::floating_point T, std::size_t D>
    struct make_polygon_attribute<sequoia::maths::vec_coords<T, D, local_geometry_arena>>{
        [[nodiscard]]
        constexpr local_coordinates<T, dimensionality{D}> operator()(std::size_t i, std::size_t N) {
            return make_polygon_coordinates< T, dimensionality{D}>(i, N);
        }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension>
        requires (3 <= N) && (dimensionality{2} <= ArenaDimension) && (ArenaDimension <= dimensionality{4})
    class polygon_base{
    public:
        using value_type = T;
        using vertex_attribute_type = std::tuple<local_coordinates<T, ArenaDimension>>;
        using vertices_type         = std::array<vertex_attribute_type, N>;
        constexpr static auto num_vertices{N};
        constexpr static auto arena_dimension{ArenaDimension};

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon_base(Fn transformer, const std::optional<std::string>& label)
            : m_VBO{transformer(st_Vertices), label}
            , m_VAO{label, m_VBO}
        {
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
        constexpr static vertices_type vertices() {
            return sequoia::utilities::make_array<vertex_attribute_type, N>(
                [](std::size_t i) {
                    return make_polygon_attribute<local_coordinates<T, ArenaDimension>>{}(i, N);
                }
            );
        }

        const inline static vertices_type st_Vertices{vertices()};

        vertex_buffer_object<vertex_attribute_type> m_VBO;
        vertex_attribute_object m_VAO;
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
