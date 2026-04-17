////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/StateAwareContext/ResourcefulContext.hpp"
#include "avocet/Core/Utilities/ContainerUtilities.hpp"

#include "sequoia/Core/ContainerUtilities/Iterator.hpp"

#include <array>
#include <cassert>
#include <concepts>
#include <span>
#include <utility>

namespace avocet::opengl {
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

    template<std::size_t N>
    using raw_indices = std::array<GLuint, N>;

    class contextual_deref_policy {
        const decorated_context* m_Context{};
    protected:
        ~contextual_deref_policy() = default;

        contextual_deref_policy& operator=(const contextual_deref_policy&) = default;
    public:
        using value_type = contextual_resource_view;
        using reference  = contextual_resource_view;

        contextual_deref_policy() = default;

        explicit contextual_deref_policy(const decorated_context& ctx) : m_Context{&ctx} {}

        contextual_deref_policy(const contextual_deref_policy&) = default;

        template<std::input_or_output_iterator Iterator>
        [[nodiscard]]
        reference get(Iterator i) const {
            assert(m_Context);
            return {*m_Context, *i};
        }

        [[nodiscard]]
        friend bool operator==(const contextual_deref_policy&, const contextual_deref_policy&) noexcept = default;
    };

    template<std::size_t N>
    class contextual_resource_handles {
        using array_t = std::array<resource_handle, N>;
        context_ref m_Context;
        array_t m_Handles;

        using citer_t = array_t::const_iterator;
    public:
        using const_iterator = sequoia::utilities::iterator<citer_t, contextual_deref_policy>;

        contextual_resource_handles(const decorated_context& ctx, const raw_indices<N>& indices)
            : m_Context{ctx}
            , m_Handles{to_array(indices, [&ctx](GLuint i) { return resource_handle{i}; })}
        {}

        [[nodiscard]]
        const_iterator begin() const noexcept { return {m_Handles.begin(), context()}; }

        [[nodiscard]]
        const_iterator end() const noexcept { return {m_Handles.end(), context()}; }

        [[nodiscard]]
        raw_indices<N> get_raw_indices() const {
            return to_array(m_Handles, [](const resource_handle& crv) { return crv.index(); });
        }

        [[nodiscard]]
        const decorated_context& context() const noexcept { return m_Context.get(); }

        [[nodiscard]]
        friend bool operator==(const contextual_resource_handles&, const contextual_resource_handles&) noexcept = default;
    };

    using contextual_resource_handle = contextual_resource_handles<1>;
}
