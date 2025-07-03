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

#include "sequoia/Maths/Geometry/Spaces.hpp"

#include <vector>

namespace avocet::opengl {
    template<class T>
    struct legal_buffer_type : std::false_type {};

    template<class T>
    using legal_buffer_type_t = legal_buffer_type<T>::type;

    template<class T>
    inline constexpr bool legal_buffer_type_v{legal_buffer_type<T>::value};

    template<gl_arithmetic T>
    struct legal_buffer_type<T> : std::true_type {};

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
    private:
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

    template<buffer_species Species, class T>
        requires legal_buffer_type_v<T>
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

    template<buffer_species Species, class T>
        requires legal_buffer_type_v<T>
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

    template<class... Ts>
    class vertex_buffer_object;

    template<gl_arithmetic T>
    class vertex_buffer_object<T> : public generic_buffer_object<buffer_species::array, T> {
        friend class vertex_attribute_object;
    public:
        using generic_buffer_object<buffer_species::array, T>::generic_buffer_object;
    };

    template<class... Attributes>
        requires legal_buffer_type_v<std::tuple<Attributes...>>
    class vertex_buffer_object<Attributes...> : public generic_buffer_object<buffer_species::array, std::tuple<Attributes...>> {
        friend class vertex_attribute_object;
    public:
        using fundamental_type = std::common_type_t<typename Attributes::value_type...>;

        using generic_buffer_object<buffer_species::array, std::tuple<Attributes...>>::generic_buffer_object;
    };

    template<gl_integral T>
    class element_buffer_object : public generic_buffer_object<buffer_species::element_array, T> {
    public:
        using generic_buffer_object<buffer_species::element_array, T>::generic_buffer_object;
    };

 
    class vertex_attribute_object : public generic_resource<num_resources{1}, vao_lifecycle_events> {
    public:
        using lifecycle_events_type = vao_lifecycle_events;
        using base_type = generic_resource<num_resources{1}, vao_lifecycle_events>;

        template<class... Attributes>
        vertex_attribute_object(const optional_label& label, const vertex_buffer_object<Attributes...>& vbo)
            : base_type{{{label}}}
        {
            using fundamental_type = vertex_buffer_object<Attributes...>::fundamental_type;
            static_assert(gl_floating_point<fundamental_type>);
            vertex_buffer_object<Attributes...>::do_bind(vbo);
            next_attribute_indices nextParams{naturally_ordered<Attributes...>(), sizeof...(Attributes)};
            constexpr auto stride{(sizeof(Attributes) + ...)};
            (set_attribute_ptr<fundamental_type>(nextParams, sizeof(Attributes) / sizeof(fundamental_type), stride), ...);
        }

        friend void bind(const vertex_attribute_object& vao) { base_type::do_bind(vao); }
    private:
        template<class... Attributes>
        [[nodiscard]]
        consteval static bool naturally_ordered() noexcept {
            std::tuple<Attributes...> t;
            return static_cast<void*>(&t) == static_cast<void*>(&std::get<0>(t));
        }

        class next_attribute_indices {
            bool m_NaturallyOrdered{};
            GLuint m_Index{};
            std::size_t m_Offset{};
        public:
            next_attribute_indices(bool naturallyOrdered, GLuint numAttributes)
                : m_NaturallyOrdered{naturallyOrdered}
                , m_Index{naturallyOrdered ? 0 : numAttributes - 1}
            { }

            [[nodiscard]]
            GLuint index() const noexcept { return m_Index; }

            [[nodiscard]]
            std::size_t offset() const noexcept { return m_Offset; }

            next_attribute_indices& advance(std::size_t offsetIncrement) noexcept {
                m_NaturallyOrdered ? ++m_Index : --m_Index;
                m_Offset += offsetIncrement;
                return *this;
            }
        };

        template<class ValueType>
        static void set_attribute_ptr(next_attribute_indices& indices, GLint components, GLsizei stride) {
            constexpr auto typeSpecifier{to_gl_enum(to_gl_type_specifier_v<ValueType>)};
            if constexpr(std::is_same_v<ValueType, GLdouble>) {
                gl_function{glVertexAttribLPointer}(indices.index(), components, typeSpecifier, stride, (GLvoid*)indices.offset());
            }
            else {
                gl_function{glVertexAttribPointer}(indices.index(), components, typeSpecifier, GL_FALSE, stride, (GLvoid*)indices.offset());
            }
            gl_function{glEnableVertexAttribArray}(indices.index());

            indices.advance(components * sizeof(ValueType));
        }
    };


}
