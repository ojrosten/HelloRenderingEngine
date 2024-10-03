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
    using gl_index_array = std::array<GLuint, N>;

    template<std::size_t N>
    using gl_handle_array = std::array<resource_handle, N>;

    template<std::size_t N>
    gl_handle_array<N> to_handle_array(const gl_index_array<N>& indices) {
        return[&] <std::size_t... Is> (std::index_sequence<Is...>){ return gl_handle_array<N>{resource_handle{indices[Is]}...}; }(std::make_index_sequence<N>{});
    }

    template<std::size_t N>
    gl_index_array<N> to_index_array(const gl_handle_array<N>& handles) {
        return[&] <std::size_t... Is> (std::index_sequence<Is...>){ return gl_index_array<N>{handles[Is].index()...}; }(std::make_index_sequence<N>{});
    }

    struct num_resources { std::size_t value{}; };

    template<num_resources NumResources, class LifeEvents>
    struct generated_resource_lifecycle {
        constexpr static auto N{NumResources.value};

        [[nodiscard]]
        static gl_handle_array<N> generate() {
            gl_index_array<N> indices{};
            LifeEvents::generate(indices);
            return to_handle_array(indices);
        }

        static void destroy(const gl_handle_array<N>& handles) {
            const auto indices{to_index_array(handles)};
            LifeEvents::destroy(indices);
        }
    };

    template<num_resources N, class T>
    inline constexpr bool has_lifecycle_events_v{
        requires(T& t, gl_index_array<N.value>& indices) {
            t.generate(indices);
            t.destroy(indices);
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

        const resource_handle& handle() const noexcept requires (N.value==1) { return m_Handles[0]; }

        [[nodiscard]]
        friend bool operator==(const resource&, const resource&) noexcept = default;
    private:
        gl_handle_array<N.value> m_Handles;
    };
}