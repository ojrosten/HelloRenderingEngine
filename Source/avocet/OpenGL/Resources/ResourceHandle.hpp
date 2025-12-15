////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"
#include "sequoia/Core/Meta/Concepts.hpp"
#include "sequoia/Core/ContainerUtilities/Iterator.hpp"

#include <array>
#include <cassert>
#include <concepts>
#include <span>
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
        const decorated_context* m_Context{};
    public:
        context_ref(const decorated_context& ctx)
            : m_Context{&ctx}
        {
        }

        context_ref(context_ref&&) noexcept = default;

        context_ref& operator=(context_ref&& other) noexcept {
            std::ranges::swap(m_Context, other.m_Context);
            return *this;
        }

        [[nodiscard]]
        const decorated_context& get() const noexcept { return *m_Context; }

        [[nodiscard]]
        friend bool operator==(const context_ref&, const context_ref&) noexcept = default;
    };

    class contextual_resource_handle {
        context_ref m_Context;
        resource_handle m_Handle;
    public:
        contextual_resource_handle(const decorated_context& ctx, resource_handle h)
            : m_Context{ctx}
            , m_Handle{std::move(h)}
        {
        }

        [[nodiscard]]
        const decorated_context& context() const noexcept { return m_Context.get(); }

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        friend bool operator==(const contextual_resource_handle&, const contextual_resource_handle&) noexcept = default;
    };

    [[nodiscard]]
    inline GLuint get_index(const contextual_resource_handle& h) { return h.handle().index(); }

    template<std::size_t N>
    using raw_indices = std::array<GLuint, N>;

    template<class From, std::size_t N, std::invocable<From> Fn, class To = std::invoke_result_t<Fn, From>>
    [[nodiscard]]
    std::array<To, N> to_array(std::span<const From, N> from, Fn fn) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
            return std::array<To, N>{fn(from[Is])...};
        }(std::make_index_sequence<N>{});
    }

    template<class From, std::size_t N, std::invocable<From> Fn, class To = std::invoke_result_t<Fn, From>>
    [[nodiscard]]
    std::array<To, N> to_array(const std::array<From, N>& from, Fn fn) {
        return to_array(std::span(from), std::move(fn));
    }

    template<std::size_t N>
    class contextual_resource_handles {
        std::array<contextual_resource_handle, N> m_Handles;
    public:
        contextual_resource_handles(const decorated_context& ctx, const raw_indices<N>& indices)
            : m_Handles{to_array(indices, [&ctx](GLuint i) { return contextual_resource_handle{ctx, resource_handle{i}}; })}
        {}

        [[nodiscard]]
        auto begin() const noexcept { return m_Handles.begin(); }

        [[nodiscard]]
        auto end() const noexcept { return m_Handles.end(); }

        [[nodiscard]]
        raw_indices<N> get_raw_indices() const {
            return to_array(m_Handles, [](const contextual_resource_handle& h) { return get_index(h); });
        }

        [[nodiscard]]
        const decorated_context& context() const noexcept
            requires (N > 0)
        {
            return m_Handles.front().context();
        }

        [[nodiscard]]
        friend bool operator==(const contextual_resource_handles&, const contextual_resource_handles&) noexcept = default;
    };
}