////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"
#include "sequoia/Core/DataStructures/MemOrderedTuple.hpp"
#include "sequoia/Maths/Geometry/Spaces.hpp"

#include <cmath>
#include <numbers>

namespace avocet {
    struct dimensionality {
        std::size_t value{};

        [[nodiscard]]
        constexpr friend auto operator<=>(const dimensionality&, const dimensionality&) noexcept = default;
    };

    struct local_geometry_arena {};

    struct texture_arena {};

    template<std::floating_point T, dimensionality D>
    using local_coordinates = sequoia::maths::vec_coords<T, D.value, local_geometry_arena>;

    template<std::floating_point T>
    using texture_coordinates = sequoia::maths::vec_coords<T, 2, texture_arena>;

    template<std::floating_point T, dimensionality D>
    [[nodiscard]]
    constexpr local_coordinates<T, D> make_polygon_coordinates(std::size_t i, std::size_t N) {
        if(N == 0)
            throw std::domain_error{"make_polygon_coordinates: The number of polygon vertices must be greater than zero"};

        constexpr T pi{std::numbers::pi_v<T>};
        const auto offset{N % 2 ? 0 : pi / N};

        const auto theta_n{offset + 2 * pi * i / N};

        return {-T{0.5} * std::sin(theta_n), T{0.5} * std::cos(theta_n)};
    }

    template<std::floating_point T>
    [[nodiscard]]
    constexpr texture_coordinates<T> make_polygon_tex_coordinates(std::size_t i, std::size_t N) {
        return texture_coordinates<T>{T{0.5}, T{0.5}} + texture_coordinates<T>{make_polygon_coordinates<T, dimensionality{2}> (i, N).values()};
    }

    template<class T>
    struct make_polygon_attribute;

    template<std::floating_point T, std::size_t D>
    struct make_polygon_attribute<sequoia::maths::vec_coords<T, D, local_geometry_arena>>{
        [[nodiscard]]
        constexpr local_coordinates<T, dimensionality{D}> operator()(std::size_t i, std::size_t N) const {
            return make_polygon_coordinates<T, dimensionality{D}>(i, N);
        }
    };

    template<std::floating_point T>
    struct make_polygon_attribute<texture_coordinates<T>> {
        [[nodiscard]]
        constexpr texture_coordinates<T> operator()(std::size_t i, std::size_t N) const {
            return make_polygon_tex_coordinates<T>(i, N);
        }
    };

    template<std::floating_point T, std::size_t N, dimensionality ArenaDimension, class... Attributes>
        requires (3 <= N) && (dimensionality{2} <= ArenaDimension)
    struct make_polygon {
        using vertex_attribute_type = sequoia::mem_ordered_tuple<local_coordinates<T, ArenaDimension>, Attributes...>;
        using vertices_type = std::array<vertex_attribute_type, N>;

        [[nodiscard]]
        constexpr std::array<vertex_attribute_type, N> operator()() const {
            return sequoia::utilities::make_array<vertex_attribute_type, N>(
                [](std::size_t i) -> vertex_attribute_type {
                    return {make_polygon_attribute<local_coordinates<T, ArenaDimension>>{}(i, N), make_polygon_attribute<Attributes>{}(i, N)...};
                }
            );
        }
    };

    
}