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

    template<class T>
    inline constexpr bool has_configurator_type_v{
        requires { typename T::configurator; }
    };

    template<class LifeEvents>
    inline constexpr bool has_configure_event_v{
           has_configurator_type_v<LifeEvents>
        && requires(const LifeEvents& lifeEvents, resourceful_contextual_resource_view crv, const LifeEvents::configurator& configurator) {
               lifeEvents.configure(crv, configurator);
           }
    };

    template<class LifeEvents>
    inline constexpr bool has_common_lifecycle_v{
           sequoia::pseudoregular<LifeEvents>
        && has_lifecycle_identifiers_v<LifeEvents>
        && has_configure_event_v<LifeEvents>
    };

    template<class LifeEvents, num_resources NumResources>
    concept standard_lifecycle_for =
           has_common_lifecycle_v<LifeEvents>
        && has_bind_event_v<LifeEvents>
        && requires(const LifeEvents& lifeEvents, raw_indices<NumResources.value>&indices, decorated_contextual_resource_view crv) {
               lifeEvents.generate(crv.context(), indices);
               lifeEvents.destroy(crv.context(), indices);
           };

    template<class LifeEvents>
    concept program_lifecycle =
           has_common_lifecycle_v<LifeEvents>
        && requires(const LifeEvents& lifeEvents, resourceful_contextual_resource_view crv) {
               { lifeEvents.create(crv.context()) } -> std::same_as<contextual_resource_handle>;
               lifeEvents.destroy(crv);
           };

    template<num_resources NumResources, class LifeEvents>
    class resource_lifecycle_base;

    template<num_resources NumResources, class LifeEvents>
        requires has_common_lifecycle_v<LifeEvents>
    class resource_lifecycle_base<NumResources, LifeEvents> {
        LifeEvents m_LifeEvents;
    public:
        using configurator_type = LifeEvents::configurator;

        constexpr static std::size_t N{NumResources.value};

        explicit resource_lifecycle_base(const LifeEvents& lifeEvents)
            : m_LifeEvents{lifeEvents}
        {
        }

        void utilize(this const resource_lifecycle_base& self, resourceful_contextual_resource_view crv)
            requires has_utilization_event_v<LifeEvents>
        {
            crv.context().utilize(self.life_events(), crv.handle());
        }

        void configure(this const resource_lifecycle_base& self, resourceful_contextual_resource_view crv, const configurator_type& config) {
            self.life_events().configure(crv, config);
        }

        template<class Self>
        void destroy(this const Self& self, const contextual_resource_handles<N>& crhs) {

        }

        [[nodiscard]]
        friend bool operator==(const resource_lifecycle_base&, const resource_lifecycle_base&) noexcept = default;
    protected:
        resource_lifecycle_base(const resource_lifecycle_base&)     = default;
        resource_lifecycle_base(resource_lifecycle_base&&) noexcept = default;

        resource_lifecycle_base& operator=(const resource_lifecycle_base&)     = default;
        resource_lifecycle_base& operator=(resource_lifecycle_base&&) noexcept = default;

        ~resource_lifecycle_base() = default;

        [[nodiscard]]
        const LifeEvents& life_events() const noexcept { return m_LifeEvents; }
    };

    template<num_resources NumResources, class LifeEvents>
    struct resource_lifecycle;

    template<num_resources NumResources, standard_lifecycle_for<NumResources> LifeEvents>
    struct resource_lifecycle<NumResources, LifeEvents> {
        using configurator_type = LifeEvents::configurator;

        constexpr static std::size_t N{NumResources.value};

        [[nodiscard]]
        static contextual_resource_handles<N> generate(const resourceful_context& ctx) {
            raw_indices<N> indices{};
            LifeEvents::generate(ctx, indices);
            return {ctx, indices};
        }

        static void destroy(const contextual_resource_handles<N>& crhs) {
            for (resourceful_contextual_resource_view crv : crhs) {
                crv.context().reset(LifeEvents{}, crv.handle());
            }
            LifeEvents::destroy(crhs.context(), crhs.get_raw_indices());
        }

        static void bind(resourceful_contextual_resource_view crv) { 
            crv.context().utilize(LifeEvents{}, crv.handle());
        }

        static void configure(decorated_contextual_resource_view crv, const configurator_type& config) {
            LifeEvents{}.configure(crv, config);
        }
    };

    template<num_resources NumResources, standard_lifecycle_for<NumResources> LifeEvents>
    class resource_wrapper{
    public:
        using lifecycle_type = resource_lifecycle<NumResources, LifeEvents>;

        constexpr static std::size_t N{NumResources.value};

        explicit resource_wrapper(const resourceful_context& ctx) : m_Handles{lifecycle_type::generate(ctx)} {}
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

    template<num_resources NumResources, standard_lifecycle_for<NumResources> LifeEvents>
        requires (NumResources.value > 0)
    class generic_resource {
        using resource_type = resource_wrapper<NumResources, LifeEvents>;
        using lifecycle_type = resource_type::lifecycle_type;
        resource_type m_Resource;
    public:
        using configurator_type = lifecycle_type::configurator_type;
        constexpr static std::size_t N{NumResources.value};

        generic_resource(const resourceful_context& ctx, const std::array<configurator_type, N>& configs)
            : m_Resource{ctx}
        {
            for(const auto& [ctxRsrc, config] : std::views::zip(contextual_handles(), configs)) {
                if(ctxRsrc.handle() == resource_handle{})
                    throw std::runtime_error{"generic_resource - null resource"};

                do_utilize(ctxRsrc);
                lifecycle_type::configure(ctxRsrc, config);
            }
        }

        [[nodiscard]]
        bool is_null() const noexcept {
            auto isNull{[](decorated_contextual_resource_view crv) { return crv.handle() == resource_handle{}; }};
            assert(std::ranges::all_of(contextual_handles(), isNull) or std::ranges::none_of(contextual_handles(), isNull));
            return isNull(contextual_handle_view(index<0>{}));
        }

        template<std::size_t I>
            requires (I < N)
        [[nodiscard]]
        std::string extract_label(index<I> i) const { return get_object_label(LifeEvents::identifier, contextual_handle_view(i)); }

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

        void do_utilize(this const generic_resource&, resourceful_contextual_resource_view crv) {
            lifecycle_type::bind(crv);
        }

        template<std::size_t I>
            requires (I < N)
        void do_utilize(this const generic_resource& self, index<I> i) { self.do_utilize(self.contextual_handle_view(i)); }

        void do_utilize(this const generic_resource& self) requires (N == 1) { self.do_utilize(index<0>{}); }

        [[nodiscard]]
        const contextual_resource_handles<N>& contextual_handles() const noexcept { return m_Resource.contextual_handles(); }

        template<std::size_t I>
            requires (I < N)
        [[nodiscard]]
        resourceful_contextual_resource_view contextual_handle_view(index<I>) const noexcept { return contextual_handles().begin()[I]; }

        [[nodiscard]]
        resourceful_contextual_resource_view contextual_handle_view() const noexcept
            requires (N == 1)
        {
            return contextual_handle_view(index<0>{});
        }
    };
}