////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Resources/ResourceHandle.hpp"
#include "avocet/OpenGL/Utilities/ObjectIdentifiers.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>
#include <span>

namespace avocet::opengl {
    struct num_resources { std::size_t value{}; };

    template<std::size_t I>
    struct index { constexpr static std::size_t value{I}; };

    template<num_resources NumResources, class T>
    inline constexpr bool has_resource_lifecycle_events_v{
        requires(raw_indices<NumResources.value>& indices, const contextual_resource_handle& h) {
            T::generate(h.context(), indices);
            T::destroy(h.context(), indices);
            { T::identifier } -> std::convertible_to<object_identifier>;
            T::bind(h);
            typename T::configurator;
            T::configure(h, std::declval<typename T::configurator>());
        }
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_resource_lifecycle_events_v<NumResources, LifeEvents>
    struct resource_lifecycle {
        using configurator_type = LifeEvents::configurator;

        constexpr static std::size_t N{NumResources.value};

        [[nodiscard]]
        static contextual_resource_handles<N> generate(const decorated_context& ctx) {
            raw_indices<N> indices{};
            LifeEvents::generate(ctx, indices);
            return {ctx, indices};
        }

        static void destroy(const contextual_resource_handles<N>& h) {
            LifeEvents::destroy(h.context(), h.get_raw_indices());
        }

        static void bind(const contextual_resource_handle& h) { LifeEvents::bind(h); }

        static void configure(const contextual_resource_handle& h, const configurator_type& config) {
            LifeEvents::configure(h, config);
        }
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_resource_lifecycle_events_v<NumResources, LifeEvents>
    class resource_wrapper{
    public:
        using lifecycle_type = resource_lifecycle<NumResources, LifeEvents>;

        constexpr static std::size_t N{NumResources.value};

        explicit resource_wrapper(const decorated_context& ctx) : m_Handles{lifecycle_type::generate(ctx)} {}
        ~resource_wrapper() { lifecycle_type::destroy(m_Handles); }

        resource_wrapper(resource_wrapper&&)           noexcept = default;
        resource_wrapper& operator=(resource_wrapper&&) noexcept = default;

        [[nodiscard]]
        const contextual_resource_handles<N>& contextual_handles() const noexcept { return m_Handles; }

        [[nodiscard]]
        friend bool operator==(const resource_wrapper&, const resource_wrapper&) noexcept = default;
    private:
        contextual_resource_handles<N> m_Handles;
    };

    template<num_resources NumResources, class LifeEvents>
        requires (NumResources.value > 0) && has_resource_lifecycle_events_v<NumResources, LifeEvents>
    class generic_resource {
        using resource_type = resource_wrapper<NumResources, LifeEvents>;
        using lifecycle_type = resource_type::lifecycle_type;
        resource_type m_Resource;
    public:
        using configurator_type = lifecycle_type::configurator_type;
        constexpr static std::size_t N{NumResources.value};

        generic_resource(const decorated_context& ctx, const std::array<configurator_type, N>& configs)
            : m_Resource{ctx}
        {
            for(const auto& [ctxHandle, config] : std::views::zip(contextual_handles(), configs)) {
                if(ctxHandle.handle() == resource_handle{})
                    throw std::runtime_error{"generic_resource  - null resource"};

                lifecycle_type::bind(ctxHandle);
                lifecycle_type::configure(ctxHandle, config);
            }
        }

        [[nodiscard]]
        bool is_null() const noexcept {
            auto isNull{[](const contextual_resource_handle& h) { return h.handle() == resource_handle{}; }};
            assert(std::ranges::all_of(contextual_handles(), isNull) or std::ranges::none_of(contextual_handles(), isNull));
            return isNull(contextual_handle(index<0>{}));
        }

        template<std::size_t I>
            requires (I < N)
        [[nodiscard]]
        std::string extract_label(index<I> i) const { return get_object_label(LifeEvents::identifier, contextual_handle(i)); }

        [[nodiscard]]
        std::string extract_label() const requires (N == 1) { return extract_label(index<0>{}); }

        [[nodiscard]]
        const decorated_context& context() const noexcept { return contextual_handles().context(); }

        [[nodiscard]]
        friend bool operator==(const generic_resource&, const generic_resource&) noexcept = default;
    protected:
        ~generic_resource() = default;

        generic_resource(generic_resource&&)            noexcept = default;
        generic_resource& operator=(generic_resource&&) noexcept = default;

        template<std::size_t I>
            requires (I < N)
        static void do_bind(const generic_resource& gbo, index<I> i) { lifecycle_type::bind(gbo.contextual_handle(i)); }

        static void do_bind(const generic_resource& gbo) requires (N == 1) { do_bind(gbo, index<0>{}); }
    private:
        [[nodiscard]]
        const contextual_resource_handles<N>& contextual_handles() const noexcept { return m_Resource.contextual_handles(); }

        template<std::size_t I>
            requires (I < N)
        const contextual_resource_handle& contextual_handle(index<I>) const noexcept { return contextual_handles().begin()[I]; }
    };
}