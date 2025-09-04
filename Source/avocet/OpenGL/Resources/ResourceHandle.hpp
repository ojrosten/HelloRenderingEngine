////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "glad/gl.h"

#include <array>
#include <concepts>
#include <utility>

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

    class context_ref {
        const GladGLContext* m_Context{};
    public:
        context_ref(const GladGLContext& ctx)
            : m_Context{&ctx}
        {
        }

        context_ref(context_ref&&) noexcept = default;

        context_ref& operator=(context_ref&& other) noexcept {
            std::ranges::swap(m_Context, other.m_Context);
            return *this;
        }

        [[nodiscard]]
        const GladGLContext& get() const noexcept { return *m_Context; }

        [[nodiscard]]
        friend bool operator==(const context_ref&, const context_ref&) noexcept = default;
    };

    class contextual_resource_handle {
        context_ref m_Context;
        resource_handle m_Handle;
    public:
        contextual_resource_handle(const GladGLContext& ctx, resource_handle h)
            : m_Context{ctx}
            , m_Handle{std::move(h)}
        {
        }

        [[nodiscard]]
        const GladGLContext& context() const noexcept { return m_Context.get(); }

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        friend bool operator==(const contextual_resource_handle&, const contextual_resource_handle&) noexcept = default;
    };

    template<std::size_t N>
    using raw_indices = std::array<GLuint, N>;

    template<std::size_t N>
    using handles = std::array<resource_handle, N>;

    template<class From, std::size_t N, std::invocable<From> Fn, class To = std::invoke_result_t<Fn, From>>
    [[nodiscard]]
    std::array<To, N> to_array(const std::array<From, N>& from, Fn fn) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
            return std::array<To, N>{fn(from[Is])...};
        }(std::make_index_sequence<N>{});
    }

    template<std::size_t N>
    [[nodiscard]]
    handles<N> to_handles(const raw_indices<N>& indices) {
        return to_array(indices, [](GLuint i) { return resource_handle{i}; });
    }

    template<std::size_t N>
    [[nodiscard]]
    raw_indices<N> to_raw_indices(const handles<N>& handles) {
        return to_array(handles, [](const resource_handle& h) { return h.index(); });
    }
}