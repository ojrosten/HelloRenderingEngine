////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Resources/Labels.hpp"
#include "avocet/OpenGL/Resources/GenericResource.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include "sequoia/Core/DataStructures/MemOrderedTuple.hpp"
#include "sequoia/Core/Meta/TypeTraits.hpp"

#include <vector>

namespace avocet::opengl {
    template<class T>
    struct gl_arithmetic_type_of {};

    template<class T>
    using gl_arithmetic_type_of_t = gl_arithmetic_type_of<T>::type;

    template<gl_arithmetic T>
    struct gl_arithmetic_type_of<T> {
        using type = T;
    };

    template<class T>
        requires sequoia::has_value_type_v<T>
    struct gl_arithmetic_type_of<T> : gl_arithmetic_type_of<typename T::value_type>{
    };

    template<class T>
    struct is_legal_gl_buffer_value_type : std::false_type {};

    template<class T>
    using is_legal_gl_buffer_value_type_t = is_legal_gl_buffer_value_type<T>::type;

    template<class T>
    inline constexpr bool is_legal_gl_buffer_value_type_v{is_legal_gl_buffer_value_type<T>::value};

    template<gl_arithmetic T>
    struct is_legal_gl_buffer_value_type<T> : std::true_type {};

    template<class... Ts>
        requires (is_legal_gl_buffer_value_type_v<Ts> && ...)
              && (sequoia::are_same_v<gl_arithmetic_type_of_t<Ts>...>)
              && (sizeof(sequoia::mem_ordered_tuple<Ts...>) == (sizeof(Ts) + ...))
    struct is_legal_gl_buffer_value_type<sequoia::mem_ordered_tuple<Ts...>> : std::true_type
    {};

    struct vao_lifecycle_events {
        constexpr static auto identifier{object_identifier::vertex_array};

        struct configurator {
            optional_label label;
        };

        template<std::size_t N>
        static void generate(const decorated_context& ctx, raw_indices<N>& indices) { gl_function{&GladGLContext::GenVertexArrays}(ctx, N, indices.data()); }

        template<std::size_t N>
        static void destroy(const decorated_context& ctx, const raw_indices<N>& indices) { gl_function{&GladGLContext::DeleteVertexArrays}(ctx, N, indices.data()); }

        static void bind(contextual_resource_view h) { gl_function{&GladGLContext::BindVertexArray}(h.context(), get_index(h)); }

        static void configure(contextual_resource_view h, const configurator& config) {
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
        static void generate(const decorated_context& ctx, raw_indices<N>& indices) { gl_function{&GladGLContext::GenBuffers}(ctx, N, indices.data()); }

        template<std::size_t N>
        static void destroy(const decorated_context& ctx, const raw_indices<N>& indices) { gl_function{&GladGLContext::DeleteBuffers}(ctx, N, indices.data()); }
    };

    template<buffer_species Species, class T>
        requires is_legal_gl_buffer_value_type_v<T>
    struct buffer_lifecycle_events : common_buffer_lifecycle_events {
        struct configurator {
            std::span<const T> buffer_data;
            optional_label label;
        };

        static void bind(contextual_resource_view h) { gl_function{&GladGLContext::BindBuffer}(h.context(), to_gl_enum(Species), get_index(h)); }

        static void configure(contextual_resource_view h, const configurator& config) {
            add_label(identifier, h, config.label);
            gl_function{&GladGLContext::BufferData}(h.context(), to_gl_enum(Species), sizeof(T) * config.buffer_data.size(), config.buffer_data.data(), GL_STATIC_DRAW);
        }
    };

    template<class T>
    class vertex_buffer_object;

    class vertex_attribute_object : public generic_resource<num_resources{1}, vao_lifecycle_events> {
    public:
        using base_type = generic_resource<num_resources{1}, vao_lifecycle_events>;

        template<class... Attributes>
        vertex_attribute_object(const binding_context& ctx, const optional_label& label, const vertex_buffer_object<sequoia::mem_ordered_tuple<Attributes...>>& vbo)
            : base_type{ctx, {{label}}}
        {
            using vbo_t         = vertex_buffer_object<sequoia::mem_ordered_tuple<Attributes...>>;
            using fundamental_t = vbo_t::fundamental_type;

            vbo_t::do_bind(vbo);

            attrib_ptr_info info{};
            constexpr auto stride{(sizeof(Attributes) + ...)};
            (set_attribute_ptr<fundamental_t>(info, sizeof(Attributes), stride), ...);
        }

        void bind(this const vertex_attribute_object& self) { do_bind(self); }
    private:
        struct attrib_ptr_info {
            GLint index{};
            std::size_t offset{};

            void advance(std::size_t offsetIncrement) {
                ++index;
                offset += offsetIncrement;
            }
        };

        template<gl_arithmetic ValueType>
        void set_attribute_ptr(attrib_ptr_info& info, std::size_t sizeofAtt, GLsizei stride) {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<ValueType>)};
            const auto components{to_gl_int(sizeofAtt / sizeof(ValueType))};
            const auto& ctx{this->context()};
            if constexpr(std::is_same_v<ValueType, GLdouble>) {
                gl_function{&GladGLContext::VertexAttribLPointer}(ctx, info.index, components, typeSpecifier, stride, std::bit_cast<GLvoid*>(info.offset));
            }
            else {
                gl_function{&GladGLContext::VertexAttribPointer}(ctx, info.index, components, typeSpecifier, GL_FALSE, stride, std::bit_cast<GLvoid*>(info.offset));
            }
            gl_function{&GladGLContext::EnableVertexAttribArray}(ctx, info.index);

            info.advance(sizeofAtt);
        }
    };

    template<buffer_species Species, class T>
        requires is_legal_gl_buffer_value_type_v<T>
    class generic_buffer_object : public generic_resource<num_resources{1}, buffer_lifecycle_events<Species, T>>
    {
    public:
        constexpr static auto species{Species};
        using value_type = T;
        using base_type = generic_resource<num_resources{1}, buffer_lifecycle_events<Species, T>> ;

        generic_buffer_object(const binding_context& ctx, std::span<const T> data, const optional_label& label)
            : base_type{ctx, {{data, label}}}
        {}

        [[nodiscard]]
        std::vector<T> extract_data(this const generic_buffer_object& self) {
            base_type::do_bind(self);
            const auto size{get_buffer_size(self.context())};
            std::vector<T> buffer(size / sizeof(T));
            gl_function{&GladGLContext::GetBufferSubData}(self.context(), to_gl_enum(Species), 0, size, buffer.data());
            return buffer;
        }
    private:
        [[nodiscard]]
        static GLint get_buffer_size(const decorated_context& ctx) {
            GLint param{};
            gl_function{&GladGLContext::GetBufferParameteriv}(ctx, to_gl_enum(Species), GL_BUFFER_SIZE, &param);
            return param;
        }
    };

    template<gl_arithmetic T>
    class vertex_buffer_object<T> : public generic_buffer_object<buffer_species::array, T> {
    public:
        using generic_buffer_object<buffer_species::array, T>::generic_buffer_object;
    };

    template<class... Attributes>
        requires (is_legal_gl_buffer_value_type_v<Attributes> && ...)
    class vertex_buffer_object<sequoia::mem_ordered_tuple<Attributes...>> : public generic_buffer_object<buffer_species::array, sequoia::mem_ordered_tuple<Attributes...>> {
        friend class vertex_attribute_object;
    public:
        using generic_buffer_object<buffer_species::array, sequoia::mem_ordered_tuple<Attributes...>>::generic_buffer_object;

        using fundamental_type = std::common_type_t<gl_arithmetic_type_of_t<Attributes>...>;
    };

    template<gl_integral T>
    class element_buffer_object : public generic_buffer_object<buffer_species::element_array, T> {
    public:
        using generic_buffer_object<buffer_species::element_array, T>::generic_buffer_object;
    };

}
