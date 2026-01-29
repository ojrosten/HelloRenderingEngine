////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"

#include "avocet/OpenGL/Utilities/ObjectIdentifiers.hpp"

// TO DO: this needs untangling
#include "avocet/OpenGL/Resources/ResourceHandle.hpp"

namespace avocet::opengl {
    template<class T, class Tuple>
    struct contains_element;

    template<class T, class Tuple>
    using contains_element_t = contains_element<T, Tuple>::type;

    template<class T, class Tuple>
    inline constexpr bool contains_element_v{contains_element<T, Tuple>::value};

    template<class T, class... Ts>
    struct contains_element<T, std::tuple<Ts...>> : std::false_type {};

    template<class T, class... Ts>
        requires (std::same_as<T, Ts> || ...)
    struct contains_element<T, std::tuple<Ts...>> : std::true_type {};

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
        void bind(this const activating_context& self, const LifeEvents&, contextual_resource_view crv) {
            constexpr auto id{LifeEvents::identifier};
            if constexpr(contains_element_v<activation<object_identifier_constant<id>>, binding_tuple_t>) {
                auto& cache{std::get<activation<object_identifier_constant<id>>>(self.m_BindingCache)};
                if(cache.currently_active != crv.handle().index()) {
                    LifeEvents::bind(crv);
                    cache.currently_active = crv.handle().index();
                }
            }
            else {
                LifeEvents::bind(crv);
            }
        }

        template<class LifeEvents>
        void use(this const activating_context& self, const LifeEvents&, contextual_resource_view crv) {
            constexpr auto id{LifeEvents::identifier};
            if constexpr(contains_element_v<activation<object_identifier_constant<id>>, utilizing_tuple_t>) {
                auto& cache{std::get<activation<object_identifier_constant<id>>>(self.m_UtilizationCache)};
                if(cache.currently_active != crv.handle().index()) {
                    LifeEvents::use(crv);
                    cache.currently_active = crv.handle().index();
                }
            }
            else {
                LifeEvents::use(crv);
            }
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

        using binding_tuple_t = std::tuple<activation<object_identifier_constant<object_identifier::framebuffer>>>;
        mutable binding_tuple_t m_BindingCache;

        using utilizing_tuple_t = std::tuple<activation<object_identifier_constant<object_identifier::program>>>;
        mutable utilizing_tuple_t m_UtilizationCache;

    };
}
