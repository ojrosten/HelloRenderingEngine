////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/DecoratedContext.hpp"

#include "avocet/OpenGL/ResourceInfrastructure/ObjectIdentifiers.hpp"
#include "avocet/OpenGL/ResourceInfrastructure/ContextualResourceView.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"

namespace avocet::opengl {
    struct num_resources { std::size_t value{}; };

    template<class T>
    inline constexpr bool has_bind_event_v{
        requires(contextual_resource_view crv) {
            T::bind(crv);
        }
    };

    template<class T>
    inline constexpr bool has_use_event_v{
        requires(contextual_resource_view crv) {
            T::use(crv);
        }
    };

    class resourceful_context : public decorated_context {
    public:
        using decorated_context::decorated_context;

    protected:
        ~resourceful_context() = default;

        resourceful_context(resourceful_context&&) noexcept = default;

        resourceful_context& operator=(resourceful_context&&) noexcept = default;
    private:
        template<class T>
        struct index_cache {
            GLuint currently_active{};
        };

        using tuple_t
            = std::tuple<
                index_cache<caching_identifier_constant<caching_identifier::framebuffer>>,
                index_cache<caching_identifier_constant<caching_identifier::program>>
            >;

        mutable tuple_t m_IndexCache;

        template<class LifeEvents>
        constexpr static bool requests_tracking_cache_v{
            requires(contextual_resource_view crv) {
                { LifeEvents::caching_id } -> std::convertible_to<caching_identifier>;
            }
        };

        template<class LifeEvents>
        constexpr static bool has_static_cache_for_v{
            requires {
                requires sequoia::meta::contains_v<tuple_t, index_cache<caching_identifier_constant<LifeEvents::caching_id>>>;
            }
        };

        friend class shader_program;

        template<num_resources NumResources, class LifeEvents>
        friend class generic_resource;

        template<class LifeEvents>
            requires has_bind_event_v<LifeEvents> || has_use_event_v<LifeEvents>
        void utilize(this const resourceful_context & self, const LifeEvents& lifeEvents, const resource_handle& h) {
            if constexpr(requests_tracking_cache_v<LifeEvents>) {
                static_assert(has_static_cache_for_v<LifeEvents>);

                constexpr auto id{LifeEvents::caching_id};
                auto& cache{std::get<index_cache<caching_identifier_constant<id>>>(self.m_IndexCache)};
                if(cache.currently_active != h.index()) {
                    self.do_utilize(lifeEvents, h);
                    cache.currently_active = h.index();
                }
            }
            else if constexpr(has_static_cache_for_v<LifeEvents>) {
                static_assert(false, "Add a caching_id to the relevant life events");
            }
            else {
                self.do_utilize(lifeEvents, h);
            }
        }

        template<class LifeEvents>
            requires has_bind_event_v<LifeEvents> || has_use_event_v<LifeEvents>
        void reset(this const resourceful_context& self, const LifeEvents& lifeEvents, const resource_handle& h) {
            if constexpr(requests_tracking_cache_v<LifeEvents>) {
                static_assert(has_static_cache_for_v<LifeEvents>);

                constexpr auto id{LifeEvents::caching_id};
                auto& cache{std::get<index_cache<caching_identifier_constant<id>>>(self.m_IndexCache)};
                if(cache.currently_active == h.index()) {
                    self.do_utilize(lifeEvents, resource_handle{});
                    cache.currently_active = 0;
                }
            }
            else if constexpr (has_static_cache_for_v<LifeEvents>) {
                static_assert(false, "Add a caching_id to the relevant life events");
            }
        }

        template<class LifeEvents>
            requires has_bind_event_v<LifeEvents> || has_use_event_v<LifeEvents>
        void do_utilize(this const resourceful_context & self, const LifeEvents&, const resource_handle & h) {
            contextual_resource_view crv{self, h};
            if constexpr(has_bind_event_v<LifeEvents>) {
                LifeEvents::bind(crv);
            }
            else {
                LifeEvents::use(crv);
            }
        }
    };
}
