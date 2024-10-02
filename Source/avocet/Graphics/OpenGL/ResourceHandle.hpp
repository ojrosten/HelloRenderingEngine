////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "glad/gl.h"

#include <utility>
#include <concepts>

namespace avocet::opengl {
    class resource_handle {
        GLuint m_Index{};
    public:
        resource_handle() noexcept = default;

        explicit resource_handle(GLuint index) noexcept : m_Index{index} {}

        resource_handle(resource_handle&& other) noexcept : m_Index{std::exchange(other.m_Index, 0)} {}

        resource_handle& operator=(resource_handle&& other) noexcept
        {
            std::ranges::swap(m_Index, other.m_Index);
            return *this;
        }

        [[nodiscard]]
        GLuint index() const noexcept { return m_Index; }

        [[nodiscard]]
        friend bool operator==(const resource_handle&, const resource_handle&) noexcept = default;
    };

    template<class T>
    inline constexpr bool has_shader_lifecycle_events_v{
        requires(T & t, const resource_handle& handle) {
            { t.create() } -> std::same_as<resource_handle>;
            t.destroy(handle);
        }
    };

    template<class LifeEvents>
        requires has_shader_lifecycle_events_v<LifeEvents>
    class shader_resource {
        resource_handle m_Handle;
    public:
        template<class... Args>
            requires std::is_constructible_v<LifeEvents, Args...>
        explicit(sizeof...(Args) == 1) shader_resource(const Args&... args)
            : m_Handle{LifeEvents{args...}.create()}
        {}

        ~shader_resource() { LifeEvents::destroy(m_Handle); }

        shader_resource(shader_resource&&) noexcept = default;

        shader_resource& operator=(shader_resource&&) noexcept = default;

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        friend bool operator==(const shader_resource&, const shader_resource&) noexcept = default;
    };

    struct resource_count {
        GLuint number{};
    };

    template<std::size_t I>
    struct resource_index { constexpr static auto index{I}; };

    template<GLuint N>
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

    template<std::size_t N, class LifeEvents>
    struct generated_resource_lifecycle {
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

    template<std::size_t N, class T>
    inline constexpr bool has_lifecycle_events_v{
        requires(T& t, gl_index_array<N>& indices) {
            t.generate(indices);
            t.destroy(indices);
        }
    };

    template<std::size_t N, class LifeEvents>
        requires has_lifecycle_events_v<N, LifeEvents>
    class resource {
    public:
        using lifecycle_type = generated_resource_lifecycle<N, LifeEvents>;
        constexpr static auto num_objects{N};

        resource() : m_Handles{lifecycle_type::generate()} {}

        ~resource() { lifecycle_type::destroy(m_Handles); }

        resource(resource&&) noexcept = default;

        resource& operator=(resource&&) noexcept = default;

        template<std::size_t I>
            requires (I < N)
        [[nodiscard]]
        const resource_handle& handle(resource_index<I>) const noexcept { return m_Handles[I]; }

        const resource_handle& handle() const noexcept requires (N==1) { return m_Handles[0]; }

        [[nodiscard]]
        friend bool operator==(const resource&, const resource&) noexcept = default;
    private:
        gl_handle_array<N> m_Handles;
    };
}