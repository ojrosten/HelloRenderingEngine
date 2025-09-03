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

namespace avocet::opengl {
    class resource_handle {
        GLuint m_Index{};
    public:
        resource_handle() noexcept = default;

        explicit resource_handle(GLuint index) noexcept : m_Index{ index } {}

        resource_handle(resource_handle&& other) noexcept : m_Index{ std::exchange(other.m_Index, 0) } {}

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

    class context_handle {
        const GladGLContext* m_Context{};
    public:
        explicit context_handle(const GladGLContext& ctx) noexcept : m_Context{&ctx} {}

        context_handle(context_handle&&) noexcept =  default;

        context_handle& operator=(context_handle&& other) noexcept
        {
            std::ranges::swap(m_Context, other.m_Context);
            return *this;
        }

        [[nodiscard]]
        const GladGLContext& get() const noexcept { return *m_Context; }

        [[nodiscard]]
        friend bool operator==(const context_handle&, const context_handle&) noexcept = default;
    };

    class contextual_resource {
    public:
        contextual_resource(const GladGLContext& ctx) noexcept
          : m_Context{ctx}
        {}

        contextual_resource(const GladGLContext& ctx, resource_handle h) noexcept
            : m_Context{ctx}
            , m_Handle{std::move(h)}
        {}

        [[nodiscard]]
        const resource_handle& handle() const noexcept { return m_Handle; }

        [[nodiscard]]
        const GladGLContext& context() const noexcept { return m_Context.get(); }

        [[nodiscard]]
        friend bool operator==(const contextual_resource&, const contextual_resource&) noexcept = default;
    private:
        context_handle m_Context;
        resource_handle m_Handle{};
    };

    template<std::size_t N>
    using raw_indices = std::array<GLuint, N>;


    template<class From, std::size_t N, std::invocable<From> Fn, class To=std::invoke_result_t<Fn, From>>
    [[nodiscard]]
    std::array<To, N> to_array(const std::array<From, N>& from, Fn fn) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
                return std::array<To, N>{fn(from[Is])...};
            }(std::make_index_sequence<N>{});
    }

    template<std::size_t N>
    class contextual_resources {
    public:
        contextual_resources(const GladGLContext& ctx, const raw_indices<N>& indices)
            : m_Handles{to_array(indices, [&ctx](GLuint i){ return contextual_resource{ctx, resource_handle{i}}; })}
        {}

        [[nodiscard]]
        auto begin() const noexcept { return m_Handles.begin(); }

        [[nodiscard]]
        auto end() const noexcept { return m_Handles.end(); }

        [[nodiscard]]
        raw_indices<N> get_raw_indices() const noexcept {
            return to_array(m_Handles, [](const contextual_resource& ch){ return ch.handle().index(); });
        }

        [[nodiscard]]
        const GladGLContext& context() const noexcept
            requires (N > 0)
        {
            return m_Handles[0].context();
        }

        [[nodiscard]]
        friend bool operator==(const contextual_resources&, const contextual_resources&) noexcept = default;
    private:
        std::array<contextual_resource, N> m_Handles{};
    };
}
