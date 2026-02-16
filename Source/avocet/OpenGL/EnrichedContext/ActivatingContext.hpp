////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"

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

    template<class T>
    inline constexpr bool has_tracking_identifier_v{
        requires(contextual_resource_view crv) {
            { T::tracking_id } -> std::convertible_to<tracking_identifier>;
        }
    };

    class activating_context : public decorated_context {
    public:
        template<class Fn>
        constexpr static bool is_decorator_v{std::is_invocable_r_v<void, Fn, const context&, const decorator_data&>};

        template<class Loader, class Prologue, class Epilogue>
            requires is_decorator_v<Prologue>
                  && is_decorator_v<Epilogue>
                  && std::is_invocable_r_v<GladGLContext, Loader, GladGLContext>
        activating_context(debugging_mode mode, Loader loader, Prologue prologue, Epilogue epilogue)
            : decorated_context{mode, std::move(loader), std::move(prologue), std::move(epilogue)}
        {}

        template<class LifeEvents>
            requires has_tracking_identifier_v<LifeEvents> && has_bind_event_v<LifeEvents>
        void bind(this const activating_context& self, const LifeEvents& lifeEvents, contextual_resource_view crv) {
            self.activate(lifeEvents, crv);
        }

        template<class LifeEvents>
            requires has_tracking_identifier_v<LifeEvents> && has_use_event_v<LifeEvents>
        void use(this const activating_context& self, const LifeEvents& lifeEvents, contextual_resource_view crv) {
            self.activate(lifeEvents, crv);
        }
    protected:
        ~activating_context() = default;

        activating_context(activating_context&&) noexcept = default;

        activating_context& operator=(activating_context&&) noexcept = default;
    private:
        using decorator_type = std::function<void(const context&, const decorator_data&)>;

        decorator_type m_Prologue{}, m_Epilogue{};

        template<class T>
        struct activation {
            GLuint currently_active{};
        };

        using tuple_t
            = std::tuple<
                  activation<tracking_identifier_constant<tracking_identifier::framebuffer>>,
                  activation<tracking_identifier_constant<tracking_identifier::program>>
              >;
        mutable tuple_t m_ActivationCache;

        template<class LifeEvents>
            requires has_tracking_identifier_v<LifeEvents> && (has_use_event_v<LifeEvents> || has_bind_event_v<LifeEvents>)
        void activate(this const activating_context& self, const LifeEvents&, contextual_resource_view crv) {
            constexpr auto id{LifeEvents::tracking_id};
            if constexpr(sequoia::meta::contains_v<activation<tracking_identifier_constant<id>>, tuple_t>) {
                auto& cache{std::get<activation<tracking_identifier_constant<id>>>(self.m_ActivationCache)};
                if(cache.currently_active != crv.handle().index()) {
                    do_activate(LifeEvents{}, crv);
                    cache.currently_active = crv.handle().index();
                }
            }
            else {
                do_activate(LifeEvents{}, crv);
            }
        }

        template<class LifeEvents>
            requires (has_use_event_v<LifeEvents> || has_bind_event_v<LifeEvents>)
        static void do_activate(const LifeEvents&, contextual_resource_view crv) {
            if constexpr(has_bind_event_v<LifeEvents>) {
                LifeEvents::bind(crv);
            }
            else if constexpr(has_use_event_v<LifeEvents>) {
                LifeEvents::use(crv);
            }
            else {
                static_assert(false);
            }
        }
    };
}
