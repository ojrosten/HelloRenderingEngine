////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Graphics/Resources.hpp"
#include "avocet/OpenGL/Graphics/Labels.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include <vector>

namespace avocet::opengl {
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

    class vertex_attribute_object : public generic_resource<num_resources{1}, vao_lifecycle_events> {
    public:
        using base_type = generic_resource<num_resources{1}, vao_lifecycle_events>;

        explicit vertex_attribute_object(const optional_label& label)
            : base_type{{{label}}}
        {}

        friend void bind(const vertex_attribute_object& vao) { do_bind(vao); }
    };

    template<buffer_species Species, gl_arithmetic_type T>
    class generic_buffer_object : public generic_resource<num_resources{1}, buffer_lifecycle_events<Species, T>>
    {
    public:
        constexpr static auto species{Species};
        using value_type = T;
        using base_type = generic_resource<num_resources{1}, buffer_lifecycle_events<Species, T>> ;

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
    class element_buffer_object : public generic_buffer_object<buffer_species::element_array, T> {
    public:
        using generic_buffer_object<buffer_species::element_array, T>::generic_buffer_object;
    };

}
