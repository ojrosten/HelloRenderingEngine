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
    /*enum class cached_binding {
        framebuffer
    };

    template<cached_binding CachedBinding>
    struct cached_binding_type_map;

    template<cached_binding CachedBinding>
    using cached_binding_type_map_t = cached_binding_type_map::type;

    template<cached_binding CachedBinding>
    struct cached_binding_constant : std::integral_constant<cached_binding, CachedBinding> {
    };*/

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

    template<object_identifier... CachedBindings>
    class generic_binding_context : public decorated_context {
    public:
        template<class Fn>
        constexpr static bool is_decorator_v{std::is_invocable_r_v<void, Fn, const context&, const decorator_data&>};

        template<class Loader, class Prologue, class Epilogue>
            requires is_decorator_v<Prologue>
                  && is_decorator_v<Epilogue>
                  && std::is_invocable_r_v<GladGLContext, Loader, GladGLContext>
        generic_binding_context(debugging_mode mode, Loader loader, Prologue prologue, Epilogue epilogue)
            : decorated_context{mode, std::move(loader), std::move(prologue), std::move(epilogue)}
        {}

        template<class LifeEvents>
        void bind(this const generic_binding_context& self, const LifeEvents&, contextual_resource_view crv) {
            constexpr auto id{LifeEvents::identifier};
            if constexpr(contains_element_v<binding<object_identifier_constant<id>>, tuple_t>) {
                auto& cache{std::get<binding<object_identifier_constant<id>>>(self.m_BindingCache)};
                if(cache.currently_bound != crv.handle().index()) {
                    LifeEvents::bind(crv);
                    cache.currently_bound = crv.handle().index();
                }
            }
            else {
                LifeEvents::bind(crv);
            }
        }
    protected:
        ~generic_binding_context() = default;

        generic_binding_context(generic_binding_context&&) noexcept = default;

        generic_binding_context& operator=(generic_binding_context&&) noexcept = default;
    private:
        using decorator_type = std::function<void(const context&, const decorator_data&)>;

        decorator_type m_Prologue{}, m_Epilogue{};

        template<class T>
        struct binding {
            GLuint currently_bound{};
        };

        using tuple_t = std::tuple<binding<object_identifier_constant<CachedBindings>>...>;
        mutable tuple_t m_BindingCache;
    };

    class binding_context : public generic_binding_context<object_identifier::framebuffer> {
    public:
        using generic_binding_context<object_identifier::framebuffer>::generic_binding_context;
    protected:
        ~binding_context() = default;

        binding_context(binding_context&&) noexcept = default;

        binding_context& operator=(binding_context&&) noexcept = default;
    };
}
