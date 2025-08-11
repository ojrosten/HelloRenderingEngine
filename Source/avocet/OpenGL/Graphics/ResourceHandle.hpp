////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"

#include <utility>
#include <concepts>
#include <span>

namespace avocet {
    template<class T>
        requires std::is_arithmetic_v<T>
    class gpu_resource_handle {
        T m_Index{};
    public:
        gpu_resource_handle() noexcept = default;

        explicit gpu_resource_handle(T index) noexcept : m_Index{index} {}

        gpu_resource_handle(gpu_resource_handle&& other) noexcept : m_Index{std::exchange(other.m_Index, 0)} {}

        gpu_resource_handle& operator=(gpu_resource_handle&& other) noexcept
        {
            std::ranges::swap(m_Index, other.m_Index);
            return *this;
        }

        [[nodiscard]]
        T index() const noexcept { return m_Index; }

        [[nodiscard]]
        friend bool operator==(const gpu_resource_handle&, const gpu_resource_handle&) noexcept = default;
    };
}

namespace avocet::opengl {
    using gl_handle = gpu_resource_handle<GLuint>;

    class gpu_context {
        const GladGLContext* m_Context{};
    public:
        explicit gpu_context(const GladGLContext& ctx) noexcept : m_Context{&ctx} {}

        gpu_context(gpu_context&&) noexcept =  default;

        gpu_context& operator=(gpu_context&& other) noexcept
        {
            std::ranges::swap(m_Context, other.m_Context);
            return *this;
        }

        [[nodiscard]]
        const GladGLContext& get() const noexcept { return *m_Context; }

        [[nodiscard]]
        friend bool operator==(const gpu_context&, const gpu_context&) noexcept = default;
    };

    class contextual_handle {
    public:
        using handle_type = gpu_resource_handle<GLuint>;

        contextual_handle(const GladGLContext& ctx) noexcept
          : m_Context{ctx}
        {}

        contextual_handle(const GladGLContext& ctx, handle_type h) noexcept
            : m_Context{ctx}
            , m_Handle{std::move(h)}
        {}

        [[nodiscard]]
        const handle_type& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        const GladGLContext& context() const noexcept { return m_Context.get(); }

        [[nodiscard]]
        friend bool operator==(const contextual_handle&, const contextual_handle&) noexcept = default;
    private:
        gpu_context m_Context;
        handle_type m_Handle{};
    };

    template<std::size_t N>
    using raw_indices = std::array<GLuint, N>;


    template<class To, class From, class Fn, std::size_t N>
        requires std::is_invocable_r_v<To, Fn, From>
    [[nodiscard]]
    std::array<To, N> to_array(const std::array<From, N>& from, Fn fn) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
                return std::array<To, N>{fn(from[Is])...};
            }(std::make_index_sequence<N>{});
    }

    template<std::size_t N>
    class contextual_handles {
    public:
        using handle_type = gpu_resource_handle<GLuint>;

        contextual_handles(const GladGLContext& ctx, const raw_indices<N>& indices)
            : m_Handles{to_array<contextual_handle>(indices, [&ctx](GLuint i){ return contextual_handle{ctx, handle_type{i}}; })}
        {}

        [[nodiscard]]
        std::span<const contextual_handle, N> get() const noexcept { return m_Handles; }

        [[nodiscard]]
        raw_indices<N> get_raw_indices() const noexcept {
            return to_array<GLuint>(m_Handles, [](const contextual_handle& ch){ return ch.handle().index(); });
        }

        [[nodiscard]]
        const GladGLContext& context() const noexcept
            requires (N > 0)
        {
            return m_Handles[0].context();
        }

        [[nodiscard]]
        friend bool operator==(const contextual_handles&, const contextual_handles&) noexcept = default;
    private:
        std::array<contextual_handle, N> m_Handles{};
    };
}
