////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Graphics/Labels.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <span>
#include <vector>

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

    template<num_resources NumResources, class T>
    inline constexpr bool has_vertex_lifecycle_events_v{
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
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    struct vertex_resource_lifecycle {
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

    struct vao_lifecycle_events {
        constexpr static auto identifier{object_identifier::vertex_array};

        struct configurator {
            optional_label label;
        };

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenVertexArrays}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteVertexArrays}(N, indices.data()); }

        static void bind(const resource_handle& h) { gl_function{glBindVertexArray}(h.index()); }

        static void configure(const resource_handle& h, const configurator& config) {
            add_label(identifier, h, config.label);
        }
    };

    enum class buffer_species : GLenum {
        array         = GL_ARRAY_BUFFER,
        element_array = GL_ELEMENT_ARRAY_BUFFER
    };

    struct common_buffer_lifecycle_events {
        constexpr static auto identifier{object_identifier::buffer};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenBuffers}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteBuffers}(N, indices.data()); }
    };

    template<buffer_species Species, gl_arithmetic_type T>
    struct buffer_lifecycle_events : common_buffer_lifecycle_events {
        struct configurator {
            std::span<const T> buffer_data;
            optional_label label;
        };

        static void bind(const resource_handle& h) { gl_function{glBindBuffer}(to_gl_enum(Species), h.index()); }

        static void configure(const resource_handle& h, const configurator& config) {
            add_label(identifier, h, config.label);
            gl_function{glBufferData}(to_gl_enum(Species), sizeof(T) * config.buffer_data.size(), config.buffer_data.data(), GL_STATIC_DRAW);
        }
    };

    template<std::size_t I>
    struct index { constexpr static std::size_t value{I}; };

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class vertex_resource{
    public:
        using lifecycle_type = vertex_resource_lifecycle<NumResources, LifeEvents>;

        constexpr static std::size_t N{NumResources.value};

        vertex_resource() : m_Handles{lifecycle_type::generate()} {}
        ~vertex_resource() { lifecycle_type::destroy(m_Handles); }

        vertex_resource(vertex_resource&&) noexcept = default;
        vertex_resource& operator=(vertex_resource&&) noexcept = default;

        [[nodiscard]]
        const handles<N>& get_handles() const noexcept { return m_Handles; }

        [[nodiscard]]
        friend bool operator==(const vertex_resource&, const vertex_resource&) noexcept = default;
    private:
        handles<N> m_Handles;
    };

    template<num_resources NumResources, class LifeEvents>
        requires (NumResources.value > 0) && has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class generic_vertex_object {
        using resource_type  = vertex_resource<NumResources, LifeEvents>;
        using lifecycle_type = resource_type::lifecycle_type;
        resource_type m_Resource;
    public:
        using configurator_type = lifecycle_type::configurator_type;
        constexpr static std::size_t N{NumResources.value};

        explicit generic_vertex_object(const std::array<configurator_type, N>& configs) {
            for(const auto&[handle, config] : std::views::zip(get_handles(), configs)) {
                if(handle == resource_handle{})
                    throw std::runtime_error{"generic_vertex_object  - null resource"};

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
        friend bool operator==(const generic_vertex_object&, const generic_vertex_object&) noexcept = default;
    protected:
        ~generic_vertex_object() = default;

        generic_vertex_object(generic_vertex_object&&)            noexcept = default;
        generic_vertex_object& operator=(generic_vertex_object&&) noexcept = default;

        template<std::size_t I>
            requires (I < N)
        static void do_bind(const generic_vertex_object& gbo, index<I> i) { lifecycle_type::bind(gbo.get_handle(i)); }

        static void do_bind(const generic_vertex_object& gbo) requires (N == 1) { do_bind(gbo, index<0>{}); }
    private:
        [[nodiscard]]
        const handles<N>& get_handles() const noexcept { return m_Resource.get_handles(); }

        template<std::size_t I>
            requires (I < N)
        const resource_handle& get_handle(index<I>) const noexcept { return get_handles()[I]; }
    };

    class vertex_attribute_object : public generic_vertex_object<num_resources{1}, vao_lifecycle_events> {
    public:
        using base_type = generic_vertex_object<num_resources{1}, vao_lifecycle_events>;

        explicit vertex_attribute_object(const optional_label& label)
            : base_type{{{label}}}
        {}

        friend void bind(const vertex_attribute_object& vao) { do_bind(vao); }
    };

    template<buffer_species Species, gl_arithmetic_type T>
    class generic_buffer_object : public generic_vertex_object<num_resources{1}, buffer_lifecycle_events<Species, T>>
    {
    public:
        constexpr static auto species{Species};
        using value_type = T;
        using base_type  = generic_vertex_object<num_resources{1}, buffer_lifecycle_events<Species, T>>;

        generic_buffer_object(std::span<const T> data, const optional_label& label)
            : base_type{{{data, label}}}
        {}

        [[nodiscard]]
        friend std::vector<T> extract_data(const generic_buffer_object& gbo) {
            base_type::do_bind(gbo);
            const auto size{get_buffer_size()};
            std::vector<T> buffer(size / sizeof(T));
            gl_function{glGetBufferSubData}(to_gl_enum(Species), 0, size, buffer.data());
            return buffer;
        }
    private:
        [[nodiscard]]
        static GLint get_buffer_size() {
            GLint param{};
            gl_function{glGetBufferParameteriv}(to_gl_enum(Species), GL_BUFFER_SIZE, &param);
            return param;
        }
    };

    template<gl_arithmetic_type T>
    class vertex_buffer_object : public generic_buffer_object<buffer_species::array, T> {
    public:
        using generic_buffer_object<buffer_species::array, T>::generic_buffer_object;
    };

    template<gl_arithmetic_type T>
    class element_buffer_object :public generic_buffer_object<buffer_species::element_array, T> {
    public:
        using generic_buffer_object<buffer_species::element_array, T>::generic_buffer_object;
    };
}