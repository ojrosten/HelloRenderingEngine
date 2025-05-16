////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Graphics/ObjectIdentifiers.hpp"
#include "avocet/OpenGL/Graphics/ResourceHandle.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <span>

namespace avocet::opengl {
    template<std::size_t N>
    using raw_indices = std::array<GLuint, N>;

    template<std::size_t N>
    using handles = std::array<resource_handle, N>;

    template<class From, class To, class Fn, std::size_t N>
        requires std::is_invocable_r_v<To, Fn, From>
    [[nodiscard]]
    std::array<To, N> to_array(const std::array<From, N>& from, Fn fn) {
        return
            [&] <std::size_t... Is> (std::index_sequence<Is...>) {
            return std::array<To, N>{fn(from[Is])...};
        }(std::make_index_sequence<N>{});
    }

    template<std::size_t N>
    [[nodiscard]]
    handles<N> to_handles(const raw_indices<N>& indices) {
        return to_array<GLuint, resource_handle>(indices, [](GLuint i){ return resource_handle{i}; });
    }

    template<std::size_t N>
    [[nodiscard]]
    raw_indices<N> to_raw_indices(const handles<N>& handles) {
        return to_array<resource_handle, GLuint>(handles, [](const resource_handle& h){ return h.index(); });
    }

    struct num_resources { std::size_t value{}; };

    template<std::size_t I>
    struct index { constexpr static std::size_t value{I}; };

    template<num_resources NumResources, class T>
    inline constexpr bool has_resource_lifecycle_events_v{
        requires(raw_indices<NumResources.value>& indices, const resource_handle& h) {
            T::generate(indices);
            T::destroy(indices);
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
        static handles<N> generate() {
            raw_indices<N> indices{};
            LifeEvents::generate(indices);
            return to_handles(indices);
        }

        static void destroy(const handles<N>& h) {
            LifeEvents::destroy(to_raw_indices(h));
        }

        static void bind(const resource_handle& h) { LifeEvents::bind(h); }

        static void configure(const resource_handle& h, const configurator_type& config) {
            LifeEvents::configure(h, config);
        }
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_resource_lifecycle_events_v<NumResources, LifeEvents>
    class resource_wrapper{
    public:
        using lifecycle_type = resource_lifecycle<NumResources, LifeEvents>;

        constexpr static std::size_t N{NumResources.value};

        resource_wrapper() : m_Handles{lifecycle_type::generate()} {}
        ~resource_wrapper() { lifecycle_type::destroy(m_Handles); }

        resource_wrapper(resource_wrapper&&)           noexcept = default;
        resource_wrapper& operator=(resource_wrapper&&) noexcept = default;

        [[nodiscard]]
        const handles<N>& get_handles() const noexcept { return m_Handles; }

        [[nodiscard]]
        friend bool operator==(const resource_wrapper&, const resource_wrapper&) noexcept = default;
    private:
        handles<N> m_Handles;
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

        explicit generic_resource(const std::array<configurator_type, N>& configs) {
            for(const auto& [handle, config] : std::views::zip(get_handles(), configs)) {
                if(handle == resource_handle{})
                    throw std::runtime_error{"generic_resource  - null resource"};

                lifecycle_type::bind(handle);
                lifecycle_type::configure(handle, config);
            }
        }

        [[nodiscard]]
        bool is_null() const noexcept {
            auto isNull{[](const resource_handle& h){ return h == resource_handle{}; }};
            assert(std::ranges::all_of(get_handles(), isNull) or std::ranges::none_of(get_handles(), isNull));
            return isNull(get_handle(index<0>{}));
        }

        template<std::size_t I>
            requires (I < N)
        [[nodiscard]]
        std::string extract_label(index<I> i) const { return get_object_label(LifeEvents::identifier, get_handle(i)); }

        [[nodiscard]]
        std::string extract_label() const requires (N == 1) { return extract_label(index<0>{}); }

        [[nodiscard]]
        friend bool operator==(const generic_resource&, const generic_resource&) noexcept = default;
    protected:
        ~generic_resource() = default;

        generic_resource(generic_resource&&)            noexcept = default;
        generic_resource& operator=(generic_resource&&) noexcept = default;

        template<std::size_t I>
            requires (I < N)
        static void do_bind(const generic_resource& gbo, index<I> i) { lifecycle_type::bind(gbo.get_handle(i)); }

        static void do_bind(const generic_resource& gbo) requires (N == 1) { do_bind(gbo, index<0>{}); }
    private:
        [[nodiscard]]
        const handles<N>& get_handles() const noexcept { return m_Resource.get_handles(); }

        template<std::size_t I>
            requires (I < N)
        const resource_handle& get_handle(index<I>) const noexcept { return get_handles()[I]; }
    };
}