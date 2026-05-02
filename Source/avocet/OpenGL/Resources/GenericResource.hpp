////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Resources/ContextualResource.hpp"
#include "avocet/OpenGL/ResourceInfrastructure/ObjectIdentifiers.hpp"

#include "sequoia/PlatformSpecific/Preprocessor.hpp"

#include <algorithm>
#include <cassert>
#include <ranges>

namespace avocet::opengl {
    template<std::size_t I>
    struct index { constexpr static std::size_t value{I}; };

    template<class LifeEvents>
    inline constexpr bool has_configurator_type_v{
        requires { typename LifeEvents::configurator; }
    };

    template<class LifeEvents>
    inline constexpr bool has_configure_event_v{
           std::regular<LifeEvents>
        && has_configurator_type_v<LifeEvents>
        && requires(const LifeEvents& lifeEvents, decorated_contextual_resource_view crv) {
               lifeEvents.configure(crv, std::declval<typename LifeEvents::configurator>());
           }
    };

    template<class LifeEvents>
    inline constexpr bool has_resource_lifecycle_events_v{
           has_configure_event_v<LifeEvents>
        && requires {
               { LifeEvents::identifier } -> std::convertible_to<object_identifier>;
           }
    };

    template<num_resources NumResources, class LifeEvents>
    inline constexpr bool has_buffer_like_lifecycle_events_v{
           has_resource_lifecycle_events_v<LifeEvents>
        && has_bind_event_v<LifeEvents>
        && requires(const LifeEvents& lifeEvents, raw_indices<NumResources.value>& indices, decorated_contextual_resource_view crv) {
               lifeEvents.generate(crv.context(), indices);
               lifeEvents.destroy(crv.context(), indices);
           }
    };

    template<class LifeEvents>
    inline constexpr bool has_shader_like_lifecycle_events_v{
           has_resource_lifecycle_events_v<LifeEvents>
        && has_use_event_v<LifeEvents>
        && requires(const LifeEvents& lifeEvents, contextual_resource_view crv) {
               { lifeEvents.create(crv.context()) } -> std::same_as<contextual_resource_handle>;
               lifeEvents.destroy(crv);
           }
    };


    template<class LifeEvents>
        requires has_resource_lifecycle_events_v<LifeEvents>
    class resource_lifecycle_base {
        SEQUOIA_NO_UNIQUE_ADDRESS LifeEvents m_Events;
    public:

        using configurator_type = LifeEvents::configurator;

        explicit resource_lifecycle_base(const LifeEvents& lifeEvents)
            : m_Events{lifeEvents}
        {}

        resource_lifecycle_base(const resource_lifecycle_base&)            = default;
        resource_lifecycle_base& operator=(const resource_lifecycle_base&) = default;

        void configure(this const resource_lifecycle_base& self, decorated_contextual_resource_view crv, const configurator_type& config) {
            self.life_events().configure(crv, config);
        }

        [[nodiscard]]
        const LifeEvents& life_events() const noexcept { return m_Events; }

        friend bool operator==(const resource_lifecycle_base&, const resource_lifecycle_base&) noexcept = default;
    protected:
        ~resource_lifecycle_base() = default;
    };

    template<num_resources NumResources, class LifeEvents>
    class resource_lifecycle;

    template<num_resources NumResources, class LifeEvents>
        requires has_buffer_like_lifecycle_events_v<NumResources, LifeEvents>
    class resource_lifecycle<NumResources, LifeEvents> : public resource_lifecycle_base<LifeEvents>
    {
    public:
        using resource_lifecycle_base<LifeEvents>::resource_lifecycle_base;

        constexpr static std::size_t N{NumResources.value};

        [[nodiscard]]
        contextual_resource_handles<N> make(this const resource_lifecycle& self, const resourceful_context& ctx) {
            raw_indices<N> indices{};
            self.life_events().generate(ctx, indices);
            return {ctx, indices};
        }

        void destroy(this const resource_lifecycle& self, const contextual_resource_handles<N>& crhs) {
            for (const auto& crh : crhs) {
                crh.context().reset(self.life_events(), crh.handle());
            }
        }

        void utilize(this const resource_lifecycle& self, contextual_resource_view crv) {
            crv.context().utilize(self.life_events(), crv.handle());
        }

        friend bool operator==(const resource_lifecycle&, const resource_lifecycle&) noexcept = default;
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_buffer_like_lifecycle_events_v<NumResources, LifeEvents>
    class resource_wrapper{
    public:
        using lifecycle_type = resource_lifecycle<NumResources, LifeEvents>;

        constexpr static std::size_t N{NumResources.value};

        resource_wrapper(const resourceful_context& ctx, const LifeEvents& lifeEvents)
            : m_LifeCycle{lifeEvents}
            , m_Handles{m_LifeCycle.make(ctx)}
        {}

        ~resource_wrapper() { m_LifeCycle.destroy(m_Handles); }

        resource_wrapper(resource_wrapper&&)            noexcept = default;
        resource_wrapper& operator=(resource_wrapper&&) noexcept = default;

        [[nodiscard]]
        const lifecycle_type& lifecycle() const noexcept { return m_LifeCycle; }

        [[nodiscard]]
        const contextual_resource_handles<N>& contextual_handles() const noexcept { return m_Handles; }

        [[nodiscard]]
        friend bool operator==(const resource_wrapper&, const resource_wrapper&) noexcept = default;
    private:
        SEQUOIA_NO_UNIQUE_ADDRESS lifecycle_type m_LifeCycle;
        contextual_resource_handles<N> m_Handles;
    };

    template<num_resources NumResources, class LifeEvents>
        requires    ((NumResources > num_resources{}) && has_buffer_like_lifecycle_events_v<NumResources, LifeEvents>)
                 || ((NumResource == num_resources{}) && has_shader_like_lifecycle_events_v<LifeEvents>)
    class generic_resource {
        using resource_type = resource_wrapper<NumResources, LifeEvents>;
        using lifecycle_type = resource_type::lifecycle_type;
        resource_type m_Resource;
    public:
        using configurator_type = lifecycle_type::configurator_type;
        constexpr static std::size_t N{NumResources.value};

        generic_resource(const resourceful_context& ctx, const LifeEvents& lifeEvents, const std::array<configurator_type, N>& configs)
            : m_Resource{ctx, lifeEvents}
        {
            for(const auto& [ctxRsrc, config] : std::views::zip(contextual_handles(), configs)) {
                if(ctxRsrc.handle() == resource_handle{})
                    throw std::runtime_error{"generic_resource  - null resource"};

                do_utilize(ctxRsrc);
                m_Resource.lifecycle().configure(ctxRsrc, config);
            }
        }

        [[nodiscard]]
        bool is_null() const noexcept {
            auto isNull{[](decorated_contextual_resource_view crv) { return crv.handle() == resource_handle{}; }};
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
        const resourceful_context& context() const noexcept { return contextual_handles().context(); }

        [[nodiscard]]
        friend bool operator==(const generic_resource&, const generic_resource&) noexcept = default;
    protected:
        ~generic_resource() = default;

        generic_resource(generic_resource&&)            noexcept = default;
        generic_resource& operator=(generic_resource&&) noexcept = default;

        void do_utilize(this const generic_resource& self, contextual_resource_view crv) {
            self.m_Resource.lifecycle().utilize(crv);
        }

        template<std::size_t I>
            requires (I < N)
        void do_utilize(this const generic_resource& self, index<I> i) { self.do_utilize(self.contextual_handle(i)); }

        void do_utilize(this const generic_resource& self) requires (N == 1) { self.do_utilize(index<0>{}); }

        [[nodiscard]]
        const contextual_resource_handles<N>& contextual_handles() const noexcept { return m_Resource.contextual_handles(); }

        template<std::size_t I>
            requires (I < N)
        [[nodiscard]]
        contextual_resource_view contextual_handle(index<I>) const noexcept { return contextual_handles().begin()[I]; }
    };
}