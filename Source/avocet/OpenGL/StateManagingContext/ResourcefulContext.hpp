////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Context/DecoratedContext.hpp"

#include "avocet/OpenGL/ResourceInfrastructure/ObjectIdentifiers.hpp"
#include "avocet/OpenGL/ResourceInfrastructure/ResourceHandle.hpp"

#include "sequoia/Core/Meta/TypeAlgorithms.hpp"

namespace avocet::opengl {
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
        template<class Fn>
        constexpr static bool is_decorator_v{decorated_context::is_decorator_v<Fn>};

        template<class Loader, class Prologue, class Epilogue>
            requires is_decorator_v<Prologue>
                  && is_decorator_v<Epilogue>
                  && std::is_invocable_r_v<GladGLContext, Loader, GladGLContext>
        resourceful_context(debugging_mode mode, Loader loader, Prologue prologue, Epilogue epilogue)
            : decorated_context{mode, std::move(loader), std::move(prologue), std::move(epilogue)}
        {}

        template<class LifeEvents>
            requires has_bind_event_v<LifeEvents> || has_use_event_v<LifeEvents>
        void utilize(this const resourceful_context& self, const LifeEvents&, contextual_resource_view crv) {
            if constexpr(use_tracking_cache_v<LifeEvents>) {
                constexpr auto id{LifeEvents::tracking_id};
                auto& cache{std::get<utilization<tracking_identifier_constant<id>>>(self.m_UtilizationCache)};
                if(cache.currently_active != crv.handle().index()) {
                    do_utilize(LifeEvents{}, crv);
                    cache.currently_active = crv.handle().index();
                }
            }
            else {
                do_utilize(LifeEvents{}, crv);
            }
        }
    protected:
        ~resourceful_context() = default;

        resourceful_context(resourceful_context&&) noexcept = default;

        resourceful_context& operator=(resourceful_context&&) noexcept = default;
    private:
        template<class T>
        struct utilization {
            GLuint currently_active{};
        };

        using tuple_t
            = std::tuple<
                  utilization<tracking_identifier_constant<tracking_identifier::framebuffer>>,
                  utilization<tracking_identifier_constant<tracking_identifier::program>>
              >;

        mutable tuple_t m_UtilizationCache;

        template<class LifeEvents>
        constexpr static bool use_tracking_cache_v{
            requires(contextual_resource_view crv) {
                { LifeEvents::tracking_id } -> std::convertible_to<tracking_identifier>;
                requires sequoia::meta::contains_v<tuple_t, utilization<tracking_identifier_constant<LifeEvents::tracking_id>>>;
            }
        };

        template<class LifeEvents>
            requires has_bind_event_v<LifeEvents> || has_use_event_v<LifeEvents>
        static void do_utilize(const LifeEvents&, contextual_resource_view crv) {
            if constexpr(has_bind_event_v<LifeEvents>) {
                LifeEvents::bind(crv);
            }
            else {
                LifeEvents::use(crv);
            }
        }
    };
}
