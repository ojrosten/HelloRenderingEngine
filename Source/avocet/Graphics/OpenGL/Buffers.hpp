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

        static void bind(const resource_handle& h) { LifeEvents::bind(h.index()); }

        static void destroy(const handles<N>& h) {
            LifeEvents::destroy(to_raw_indices(h));
        }
    };

    template<std::size_t I>
    struct index { constexpr static std::size_t value{I}; };

    struct vao_lifecyle_events {
        constexpr static auto identifier{object_identifier::vertex_array};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenVertexArrays}(N, indices.data()); }

        static void bind(GLuint i) { gl_function{glBindVertexArray}(i); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteVertexArrays}(N, indices.data()); }
    };

    enum class buffer_config : GLenum {
        array_buffer         = GL_ARRAY_BUFFER,
        element_array_buffer = GL_ELEMENT_ARRAY_BUFFER
    };

    template<buffer_config BufferConfig>
    struct buffer_lifecycle_events {
        constexpr static auto identifier{object_identifier::buffer};
        constexpr static auto config{BufferConfig};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenBuffers}(N, indices.data()); }

        static void bind(GLuint i) { gl_function{glBindBuffer}(to_gl_enum(BufferConfig), i); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteBuffers}(N, indices.data()); }
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class vertex_resource{
    public:
        constexpr static std::size_t N{NumResources.value};

        vertex_resource() : m_Handles{lifecycle_type::generate()} {}
        ~vertex_resource() { lifecycle_type::destroy(m_Handles); }

        vertex_resource(vertex_resource&&)            noexcept = default;
        vertex_resource& operator=(vertex_resource&&) noexcept = default;

        [[nodiscard]]
        const handles<N>& get_handles() const noexcept { return m_Handles; }

        template<std::size_t I>
            requires (I < N)
        friend void bind(const vertex_resource& resource, index<I> i) { lifecycle_type::bind(resource.get_handles()[i.value]); }

        [[nodiscard]]
        friend bool operator==(const vertex_resource&, const vertex_resource&) noexcept = default;
    private:
        using lifecycle_type = vertex_resource_lifecyle<NumResources, LifeEvents>;

        handles<N> m_Handles;
    };

    constexpr static std::optional<std::string> null_label{std::nullopt};

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class generic_vertex_object {
        using resource_type = vertex_resource<NumResources, LifeEvents>;
        resource_type m_Resource;
    public:
        constexpr static std::size_t N{NumResources.value};

        explicit generic_vertex_object(const std::optional<std::string>& label) {
            [this, &label] <std::size_t I>(std::index_sequence<I>){
                bind(*this, index<I>{});
                add_label(label);
            }(std::make_index_sequence<N>{});
        }

        template<std::size_t I>
            requires (I < N)
        friend void bind(const generic_vertex_object& gvo, index<I> i) { bind(gvo.m_Resource, i); }

        friend void bind(const generic_vertex_object& gvo) requires (N == 1) { bind(gvo, index<0>{}); }

        [[nodiscard]]
        explicit operator bool() const noexcept requires (N == 1) { return m_Resource.get_handles()[0] == resource_handle{}; }

        [[nodiscard]]
        friend bool operator==(const generic_vertex_object&, const generic_vertex_object&) noexcept = default;
    protected:
        ~generic_vertex_object() = default;

        generic_vertex_object(generic_vertex_object&&)            noexcept = default;
        generic_vertex_object& operator=(generic_vertex_object&&) noexcept = default;
    private:
        void add_label(const std::optional<std::string>& label) {
            if(label && object_labels_activated()) {
                const auto& str{label.value()};
                for(const auto& h : m_Resource.get_handles()) {
                    gl_function{glObjectLabel}(to_gl_enum(LifeEvents::identifier), h.index(), to_gl_sizei(str.size()), str.data());
                }
            }
        }
    };

    class vertex_attribute_object : public generic_vertex_object<num_resources{1}, vao_lifecyle_events> {
    public:
        using generic_vertex_object<num_resources{1}, vao_lifecyle_events>::generic_vertex_object;
    };

    template<buffer_config Config>
    class generic_buffer_object : public generic_vertex_object<num_resources{1}, buffer_lifecycle_events<Config>> {
    public:
        using base_type = generic_vertex_object<num_resources{1}, buffer_lifecycle_events<Config>> ;

        template<class T, std::size_t N>
        generic_buffer_object(std::span<T, N> bufferData, const std::optional<std::string>& label)
            : base_type{label}
        {
            gl_function{glBufferData}(to_gl_enum(Config), sizeof(T) * N, bufferData.data(), GL_STATIC_DRAW);
        }

        template<class T>
        generic_buffer_object(std::span<T> bufferData, const std::optional<std::string>& label)
            : base_type{label}
        {
            gl_function{glBufferData}(to_gl_enum(Config), sizeof(T) * bufferData.size(), bufferData.data(), GL_STATIC_DRAW);
        }
    protected:
        ~generic_buffer_object() = default;

        generic_buffer_object(generic_buffer_object&&)            noexcept = default;
        generic_buffer_object& operator=(generic_buffer_object&&) noexcept = default;
    };

    class vertex_buffer_object  : public generic_buffer_object<buffer_config::array_buffer> {
    public:
        using generic_buffer_object<buffer_config::array_buffer>::generic_buffer_object;
    };

    class element_buffer_object : public generic_buffer_object<buffer_config::element_array_buffer> {
    public:
        using generic_buffer_object<buffer_config::element_array_buffer>::generic_buffer_object;
    };
}