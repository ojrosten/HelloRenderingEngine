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

#include <ranges>
#include <array>
#include <optional>
#include <vector>
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
        requires(raw_indices<NumResources.value>& indices, const resource_handle& h) {
            typename T::configurator;
            T::generate(indices);
            T::destroy(indices);
            { T::identifier } -> std::convertible_to<object_identifier>;
            T::bind(h);
            T::configure(h, std::declval<typename T::configurator>());
        }
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    struct vertex_resource_lifecyle {
        using configurator_type = LifeEvents::configurator;
        constexpr static std::size_t N{NumResources.value};

        [[nodiscard]]
        static handles<N> generate() {
            raw_indices<N> indices{};
            LifeEvents::generate(indices);
            return to_handles(indices);
        }

        static void bind(const resource_handle& handle) { LifeEvents::bind(handle); }

        static void configure(const resource_handle& handle, const configurator_type& init) { LifeEvents::configure(handle, init); }

        static void destroy(const handles<N>& h) {
            LifeEvents::destroy(to_raw_indices(h));
        }
    };

    template<std::size_t I>
    struct index { constexpr static std::size_t value{I}; };

    inline void add_label(object_identifier identifier, const resource_handle& handle, const std::optional<std::string>& label) {
        if(label && object_labels_activated()) {
            const auto& str{label.value()};
            gl_function{glObjectLabel}(to_gl_enum(identifier), handle.index(), to_gl_sizei(str.size()), str.data());
        }
    }

    struct vao_lifecycle_events {
        constexpr static auto identifier{object_identifier::vertex_array};

        struct configurator {
            std::optional<std::string> label;
        };

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenVertexArrays}(N, indices.data()); }

        static void bind(const resource_handle& handle) { gl_function{glBindVertexArray}(handle.index()); }

        static void configure(const resource_handle& handle, const configurator& init) { add_label(identifier, handle, init.label); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteVertexArrays}(N, indices.data()); }
    };

    enum class buffer_config : GLenum {
        array_buffer         = GL_ARRAY_BUFFER,
        element_array_buffer = GL_ELEMENT_ARRAY_BUFFER
    };

    struct buffer_lifecycle_events_base {
        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenBuffers}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteBuffers}(N, indices.data()); }
    };

    template<buffer_config BufferConfig, class T>
    struct buffer_lifecycle_events : buffer_lifecycle_events_base {
        constexpr static auto identifier{object_identifier::buffer};
        constexpr static auto config{BufferConfig};

        struct configurator {
            std::span<T> buffer_data;
            std::optional<std::string> label;
        };

        static void bind(const resource_handle& handle) { gl_function{glBindBuffer}(to_gl_enum(BufferConfig), handle.index()); }

        static void configure(const resource_handle& handle, const configurator& init) {
            add_label(identifier, handle, init.label);
            gl_function{glBufferData}(to_gl_enum(config), sizeof(T) * init.buffer_data.size(), init.buffer_data.data(), GL_STATIC_DRAW);
        }
    };

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class vertex_resource{
    public:
        using lifecycle_type = vertex_resource_lifecyle<NumResources, LifeEvents>;

        constexpr static std::size_t N{NumResources.value};

        vertex_resource() : m_Handles{lifecycle_type::generate()} {}
        ~vertex_resource() { lifecycle_type::destroy(m_Handles); }

        vertex_resource(vertex_resource&&)            noexcept = default;
        vertex_resource& operator=(vertex_resource&&) noexcept = default;

        [[nodiscard]]
        const handles<N>& get_handles() const noexcept { return m_Handles; }

        [[nodiscard]]
        friend bool operator==(const vertex_resource&, const vertex_resource&) noexcept = default;
    private:
        handles<N> m_Handles;
    };

    constexpr static std::optional<std::string> null_label{std::nullopt};

    template<num_resources NumResources, class LifeEvents>
        requires has_vertex_lifecycle_events_v<NumResources, LifeEvents>
    class generic_vertex_object {
        using resource_type = vertex_resource<NumResources, LifeEvents>;
        resource_type m_Resource;
    public:
        using configurator_type = LifeEvents::configurator;

        constexpr static std::size_t N{NumResources.value};

        explicit generic_vertex_object(const std::array<configurator_type, N>& configurators) {
            for(auto [handle, configurator] : std::views::zip(get_handles(), configurators)) {
                lifecycle_type::bind(handle);
                lifecycle_type::configure(handle, configurator);
            }
        }

        [[nodiscard]]
        explicit operator bool() const noexcept requires (N == 1) { return get_handles()[0] == resource_handle{}; }

        [[nodiscard]]
        friend bool operator==(const generic_vertex_object&, const generic_vertex_object&) noexcept = default;
    protected:
        ~generic_vertex_object() = default;

        generic_vertex_object(generic_vertex_object&&)            noexcept = default;
        generic_vertex_object& operator=(generic_vertex_object&&) noexcept = default;

        template<std::size_t I>
            requires (I < N)
        static void bind(const generic_vertex_object& gvo, index<I> i) { lifecycle_type::bind(gvo.get_handles()[i.value]); }

        static void bind(const generic_vertex_object& gvo) requires (N == 1) { bind(gvo, index<0>{}); }
    private:
        using lifecycle_type = resource_type::lifecycle_type;

        [[nodiscard]]
        const handles<N>& get_handles() const noexcept { return m_Resource.get_handles(); }
    };

    class vertex_attribute_object : public generic_vertex_object<num_resources{1}, vao_lifecycle_events>{
        using configurator_type = vao_lifecycle_events::configurator;
    public:
        using base_type = generic_vertex_object<num_resources{1}, vao_lifecycle_events> ;

        explicit vertex_attribute_object(const std::optional<std::string>& label)
            : base_type{std::array{configurator_type{label}}}
        {}

        friend void bind(const vertex_attribute_object& vao) { base_type::bind(vao); }
    };

    template<buffer_config Config, class T>
    class generic_buffer_object : public generic_vertex_object<num_resources{1}, buffer_lifecycle_events<Config, T>>{
        using configurator_type  = buffer_lifecycle_events<Config, T>::configurator;
    public:
        using base_type = generic_vertex_object < num_resources{1}, buffer_lifecycle_events<Config, T >>;

        generic_buffer_object(std::span<T> bufferData, const std::optional<std::string>& label)
            : base_type{std::array{configurator_type{bufferData, label}}}
        {}

        [[nodiscard]]
        friend std::vector<T> get_buffer_sub_data(const generic_buffer_object& buffer) {
            base_type::bind(buffer);
            const auto size{get_buffer_size()};
            std::vector<T> recoveredBuffer(size / sizeof(T));
            avocet::opengl::gl_function{glGetBufferSubData}(to_gl_enum(Config), 0, size, recoveredBuffer.data());
            return recoveredBuffer;
        }
    protected:
        ~generic_buffer_object() = default;

        generic_buffer_object(generic_buffer_object&&)            noexcept = default;
        generic_buffer_object& operator=(generic_buffer_object&&) noexcept = default;
    private:
        [[nodiscard]]
        static GLint get_buffer_size() {
            GLint param{};
            avocet::opengl::gl_function{glGetBufferParameteriv}(to_gl_enum(Config), GL_BUFFER_SIZE, &param);
            return param;
        }
    };

    template<class T>
    class vertex_buffer_object  : public generic_buffer_object<buffer_config::array_buffer, T> {
    public:
        using generic_buffer_object<buffer_config::array_buffer, T>::generic_buffer_object;
    };

    template<class T>
    class element_buffer_object : public generic_buffer_object<buffer_config::element_array_buffer, T> {
    public:
        using generic_buffer_object<buffer_config::element_array_buffer, T>::generic_buffer_object;
    };
}