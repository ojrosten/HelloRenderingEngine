////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/ResourceHandle.hpp"

namespace avocet::opengl {
    template<std::size_t I>
    struct resource_index { constexpr static auto index{I}; };

    template<std::size_t N>
    using raw_indices = std::array<GLuint, N>;

    template<std::size_t N>
    using handles = std::array<resource_handle, N>;

    template<std::size_t N>
    [[nodiscard]]
    handles<N> to_handle_array(const raw_indices<N>& indices) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
                return handles<N>{resource_handle{indices[Is]}...};
            }(std::make_index_sequence<N>{});
    }

    template<std::size_t N>
    [[nodiscard]]
    raw_indices<N> to_index_array(const handles<N>& handles) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
                return raw_indices<N>{handles[Is].index()...};
            }(std::make_index_sequence<N>{});
    }

    struct num_resources { std::size_t value{}; };

    template<num_resources NumResources, class LifeEvents>
    struct generated_resource_lifecycle {
        constexpr static auto N{NumResources.value};

        [[nodiscard]]
        static handles<N> generate() {
            raw_indices<N> indices{};
            LifeEvents::generate(indices);
            return to_handle_array(indices);
        }

        static void destroy(const handles<N>& handles) {
            const auto indices{to_index_array(handles)};
            LifeEvents::destroy(indices);
        }
    };

    template<num_resources N, class T>
    inline constexpr bool has_lifecycle_events_v{
        requires(raw_indices<N.value>& indices) {
            T::generate(indices);
            T::destroy(indices);
        }
    };

    template<num_resources N, class LifeEvents>
        requires has_lifecycle_events_v<N, LifeEvents>
    class resource {
    public:
        using lifecycle_type = generated_resource_lifecycle<N, LifeEvents>;
        constexpr static auto resource_count{N};

        resource() : m_Handles{lifecycle_type::generate()} {}

        ~resource() { lifecycle_type::destroy(m_Handles); }

        resource(resource&&) noexcept = default;

        resource& operator=(resource&&) noexcept = default;

        template<std::size_t I>
            requires (I < N.value)
        [[nodiscard]]
        const resource_handle& handle(resource_index<I>) const noexcept { return m_Handles[I]; }

        [[nodiscard]]
        const resource_handle& handle() const noexcept requires (N.value==1) { return m_Handles[0]; }

        [[nodiscard]]
        friend bool operator==(const resource&, const resource&) noexcept = default;
    private:
        handles<N.value> m_Handles;
    };

    struct vbo_lifecycle_events {
        template<std::size_t N>
        static void generate(raw_indices<N>& indices)      { glGenBuffers(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { glDeleteBuffers(N, indices.data()); }
    };


    struct vao_lifecycle_events {
        template<std::size_t N>
        static void generate(raw_indices<N>& indices)      { glGenVertexArrays(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { glDeleteVertexArrays(N, indices.data()); }
    };

    using vbo_resource = resource<num_resources{1}, vbo_lifecycle_events>;
    using vao_resource = resource<num_resources{1}, vao_lifecycle_events>;
}