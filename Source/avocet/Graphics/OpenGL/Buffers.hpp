////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/ObjectIdentifiers.hpp"
#include "avocet/Graphics/OpenGL/ResourceHandle.hpp"
#include "avocet/Graphics/OpenGL/GLFunction.hpp"
#include "avocet/Utilities/OpenGL/Casts.hpp"

#include <array>
#include <optional>

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
        requires(raw_indices<NumResources.value>& indices) {
            T::generate(indices);
            T::destroy(indices);
            { T::identifier } -> std::convertible_to<object_identifier>;
        }
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    struct vertex_resource_lifecyle {
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
    };

    struct vao_lifecyle_events {
        constexpr static auto identifier{object_identifier::vertex_array};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenVertexArrays}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteVertexArrays}(N, indices.data()); }
    };

    struct buffer_lifecyle_events {
        constexpr static auto identifier{object_identifier::buffer};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenBuffers}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteBuffers}(N, indices.data()); }
    };

    template<std::size_t I>
    struct index { constexpr static std::size_t value{I}; };

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class vertex_resource{
    public:
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
        using lifecycle_type = vertex_resource_lifecyle<NumResources, LifeEvents>;

        handles<N> m_Handles;
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class generic_vertex_object {
        using resource_type = vertex_resource<NumResources, LifeEvents>;
        resource_type m_Resource;
    public:
        constexpr static std::size_t N{NumResources.value};

        generic_vertex_object() = default;

        void add_label(const std::optional<std::string>& label) {
            if(label && object_labels_activated()) {
                const auto& str{label.value()};
                for(const auto& h : m_Resource.get_handles()) {
                    gl_function{glObjectLabel}(to_gl_enum(LifeEvents::identifier), h.index(), to_gl_sizei(str.size()), str.data());
                }
            }
        }

        template<std::size_t I>
            requires (I < N)
        [[nodiscard]]
        const resource_handle& get_handle(index<I>) const noexcept { return m_Resource.get_handles()[I]; }

        [[nodiscard]]
        const resource_handle& get_handle() const noexcept requires (N == 1) { return m_Resource.get_handles()[0]; }

        [[nodiscard]]
        friend bool operator==(const generic_vertex_object&, const generic_vertex_object&) noexcept = default;
    protected:
        ~generic_vertex_object() = default;

        generic_vertex_object(generic_vertex_object&&)            noexcept = default;
        generic_vertex_object& operator=(generic_vertex_object&&) noexcept = default;
    };

    class vertex_attribute_object : public generic_vertex_object<num_resources{1}, vao_lifecyle_events> {
    public:
        using generic_vertex_object<num_resources{1}, vao_lifecyle_events>::generic_vertex_object;
    };

    class vertex_buffer_object : public generic_vertex_object<num_resources{1}, buffer_lifecyle_events> {
    public:
        using generic_vertex_object<num_resources{1}, buffer_lifecyle_events>::generic_vertex_object;
    };

    class element_buffer_object :public generic_vertex_object<num_resources{1}, buffer_lifecyle_events> {
    public:
        using generic_vertex_object<num_resources{1}, buffer_lifecyle_events>::generic_vertex_object;
    };

    template<class Resource>
    inline constexpr bool has_single_resource_v{
        requires(const Resource & r) {
            { r.get_handle() } -> std::same_as<const resource_handle&>;
        }
    };

    template<class Resource>
        requires has_single_resource_v<Resource>
    [[nodiscard]]
    GLuint get_raw_index(const Resource& r) { return r.get_handle().index(); }
}