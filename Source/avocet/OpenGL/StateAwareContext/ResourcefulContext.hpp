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
        constexpr static bool opts_in_to_caching_v{
            requires(contextual_resource_view crv) {
                { LifeEvents::caching_id } -> std::convertible_to<caching_identifier>;
            }
        };

        template<class LifeEvents>
        constexpr static bool has_cache_v{
            sequoia::meta::contains_v<tuple_t, index_cache<caching_identifier_constant<LifeEvents::caching_id>>>
        };

        template<class LifeEvents>
        constexpr static bool has_utilization_event_v{
            has_bind_event_v<LifeEvents> || has_use_event_v<LifeEvents>
        };

        friend class shader_program;

        template<num_resources NumResources, class LifeEvents>
        friend class generic_resource;

        template<class LifeEvents>
            requires has_utilization_event_v<LifeEvents> 
        void utilize(this const resourceful_context & self, const LifeEvents& lifeEvents, const resource_handle& h) {
            if constexpr(opts_in_to_caching_v<LifeEvents>) {
                if(auto& cache{self.get_cache(lifeEvents)}; cache.currently_active != h.index()) {
                    self.cached_utlization(lifeEvents, cache, h);
                }
            }
            else {
                self.do_utilize(lifeEvents, h);
            }
        }

        template<class LifeEvents>
            requires has_utilization_event_v<LifeEvents>
        void reset(this const resourceful_context& self, const LifeEvents& lifeEvents, const resource_handle& h) {
            if constexpr(opts_in_to_caching_v<LifeEvents>) {
                if(auto& cache{self.get_cache(lifeEvents)}; cache.currently_active == h.index()) {
                    self.cached_utlization(lifeEvents, cache, resource_handle{});
                }
            }
        }

        template<class LifeEvents>
        [[nodiscard]]
        auto& get_cache(this const resourceful_context& self, const LifeEvents&) {
            static_assert(has_cache_v<LifeEvents>);

            constexpr auto id{LifeEvents::caching_id};
            using cache_t = index_cache<caching_identifier_constant<id>>;

            return std::get<cache_t>(self.m_IndexCache);
        }

        template<class LifeEvents, caching_identifier id>
            requires has_utilization_event_v<LifeEvents>
        void cached_utlization(this const resourceful_context& self, const LifeEvents& lifeEvents, index_cache<caching_identifier_constant<id>>& cache, const resource_handle& h) {
            self.do_utilize(lifeEvents, h);
            cache.currently_active = h.index();
        }

        template<class LifeEvents>
            requires has_utilization_event_v<LifeEvents>
        void do_utilize(this const resourceful_context& self, const LifeEvents&, const resource_handle& h) {
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
