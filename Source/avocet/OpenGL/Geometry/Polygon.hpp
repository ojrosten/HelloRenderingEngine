////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Geometry/PolygonCoordinates.hpp"

#include "avocet/OpenGL/Resources/Buffers.hpp"
#include "avocet/OpenGL/Resources/Textures.hpp"
#include "sequoia/PlatformSpecific/Preprocessor.hpp"

#include <limits>

namespace avocet::opengl {
    template<std::floating_point T, std::size_t D, class Arena>
    struct is_legal_gl_buffer_value_type<sequoia::maths::vec_coords<T, D, Arena>>
        : std::bool_constant<sizeof(sequoia::maths::vec_coords<T, D, Arena>) == D * sizeof(T)>
    {
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, class... Attributes>
        requires (3 <= N) && (dimensionality{2} <= ArenaDimension) && (ArenaDimension <= dimensionality{4}) && (is_legal_gl_buffer_value_type_v<Attributes> && ...)
    class polygon_base{
    public:
        using value_type = T;
        using vertex_attribute_type = sequoia::mem_ordered_tuple<local_coordinates<T, ArenaDimension>, Attributes...>;
        using vertices_type         = std::array<vertex_attribute_type, N>;
        constexpr static auto num_vertices{N};
        constexpr static auto arena_dimension{ArenaDimension};
        constexpr static std::size_t num_textures{(std::same_as<Attributes, texture_coordinates<gl_arithmetic_type_of_t<Attributes>>> + ... + 0)};

        template<class Fn>
          requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (num_textures == 0)
        polygon_base(const resourceful_context& ctx, Fn transformer, const std::optional<std::string>& label)
            : m_VBO{ctx, transformer(st_Vertices), label}
            , m_VAO{ctx, label, m_VBO}
        {
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (num_textures == 1)
        polygon_base(const resourceful_context& ctx, Fn transformer, const texture_2d_configurator& texConfig, const std::optional<std::string>& label)
            :      m_VBO{ctx, transformer(st_Vertices), label}
            ,      m_VAO{ctx, label, m_VBO}
            , m_Textures{texture_2d{ctx, texConfig}}
        {
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon_base(const resourceful_context& ctx, Fn transformer, std::span<const texture_2d_configurator, num_textures> texConfigs, const std::optional<std::string>& label)
            : m_VBO{ctx, transformer(st_Vertices), label}
            , m_VAO{ctx, label, m_VBO}
            , m_Textures{to_array(texConfigs, [&ctx](const texture_2d_configurator& config) { return texture_2d{ctx, config}; })}
        {
        }

        template<class Self>
            requires (num_textures == 0)
        void draw(this const Self& self) {
            self.bind_vao_and_draw();
        }

        template<class Self>
            requires (num_textures == 1)
        void draw(this const Self& self, texture_unit unit) {
            self.m_Textures.front().bind(unit);
            self.bind_vao_and_draw();
        }

        template<class Self>
        void draw(this const Self& self, std::span<const texture_unit, num_textures> units) {
          for(const auto[texture, unit] : std::views::zip(self.m_Textures, units))
                texture.bind(unit);

            self.bind_vao_and_draw();
        }

        [[nodiscard]]
        friend bool operator==(const polygon_base&, const polygon_base&) noexcept = default;
    protected:
        ~polygon_base() = default;

        polygon_base(polygon_base&&)            noexcept = default;
        polygon_base& operator=(polygon_base&&) noexcept = default;
    private:
        const inline static vertices_type st_Vertices{make_polygon<T, N, ArenaDimension, Attributes...>{}()};

        vertex_buffer_object<vertex_attribute_type> m_VBO;
        vertex_attribute_object m_VAO;
        SEQUOIA_NO_UNIQUE_ADDRESS std::array<texture_2d, num_textures> m_Textures;

        template<class Self>
        void bind_vao_and_draw(this const Self& self) {
            self.m_VAO.bind();
            self.do_draw(self.m_VAO.context());
        }
    };

    template<gl_floating_point T, std::size_t N, dimensionality ArenaDimension, class... Attributes>
        requires (N <= 87) && (is_legal_gl_buffer_value_type_v<Attributes> && ...)
    class polygon : public polygon_base<T, N, ArenaDimension, Attributes...> {
    public:
        using polygon_base_type = polygon_base<T, N, ArenaDimension, Attributes...>;
        using vertices_type     = polygon_base_type::vertices_type;
        constexpr static std::size_t num_textures{polygon_base_type::num_textures};

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (num_textures == 0)
        polygon(const resourceful_context& ctx, Fn transformer, const std::optional<std::string>& label)
            : polygon_base_type{ctx, transformer, label}
            ,             m_EBO{ctx, st_Indices, label}
        {
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type> && (num_textures == 1)
        polygon(const resourceful_context& ctx, Fn transformer, const texture_2d_configurator& texConfig, const std::optional<std::string>& label)
            : polygon_base_type{ctx, transformer, texConfig, label}
            ,             m_EBO{ctx, st_Indices, label}
        {
        }

        template<class Fn>
            requires std::is_invocable_r_v<vertices_type, Fn, vertices_type>
        polygon(const resourceful_context& ctx, Fn transformer, std::span<const texture_2d_configurator, num_textures> texConfigs, const std::optional<std::string>& label)
            : polygon_base_type{ctx, transformer, texConfigs, label}
            , m_EBO{ctx, st_Indices, label}
        {
        }
    private:
        friend polygon_base_type;

        using element_index_type = GLubyte;
        constexpr static auto num_elements{3 * (N - 2)};
        using element_array_type = std::array<element_index_type, num_elements>;

        static_assert(num_elements <= std::numeric_limits<element_index_type>::max());

        [[nodiscard]]
        constexpr static element_index_type to_element_index(std::size_t i) noexcept {
            const auto remainder{i % 3};
            if(!remainder)
                return 0;

            return checked_conversion_to<element_index_type>(i / 3 + remainder);
        }

        constexpr static element_array_type st_Indices{
            sequoia::utilities::make_array<element_index_type, num_elements>(to_element_index)
        };

        static void do_draw(const decorated_context& ctx) {
            gl_function{&GladGLContext::DrawElements}(ctx, GL_TRIANGLES, num_elements, to_gl_enum(to_gl_type_specifier_v<element_index_type>), nullptr);
        }

        element_buffer_object<element_index_type> m_EBO;

    };

    template<gl_floating_point T, dimensionality ArenaDimension, class... Attributes>
        requires (is_legal_gl_buffer_value_type_v<Attributes> && ...)
    class polygon<T, 3, ArenaDimension, Attributes...> : public polygon_base<T, 3, ArenaDimension, Attributes...> {
    public:
        using polygon_base_type = polygon_base<T, 3, ArenaDimension, Attributes...>;
        using polygon_base<T, 3, ArenaDimension, Attributes...>::polygon_base;
    private:
        friend polygon_base_type;

        static void do_draw(const decorated_context& ctx) {
            gl_function{&GladGLContext::DrawArrays}(ctx, GL_TRIANGLES, 0, 3);
        }
    };

    template<gl_floating_point T, dimensionality ArenaDimension, class... Attributes>
    using triangle = polygon<T, 3, ArenaDimension, Attributes...>;

    template<gl_floating_point T, dimensionality ArenaDimension, class... Attributes>
    using quad = polygon<T, 4, ArenaDimension, Attributes...>;
}
