////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/AssetManagement/Image.hpp"

#include "avocet/OpenGL/Graphics/Labels.hpp"
#include "avocet/OpenGL/Graphics/Resources.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include <array>
#include <filesystem>
#include <functional>
#include <ranges>
#include <span>
#include <vector>

namespace avocet::opengl {
    enum class colour_space_flavour : int { linear, srgb };

    enum class texture_internal_format : GLint {
        red   = GL_RED,
        rg    = GL_RG,
        rgb   = GL_RGB,
        srgb  = GL_SRGB,
        rgba  = GL_RGBA,
        srgba = GL_SRGB_ALPHA
    };

    enum class texture_format : GLenum {
        red  = GL_RED,
        rg   = GL_RG,
        rgb  = GL_RGB,
        rgba = GL_RGBA
    };

    [[nodiscard]]
    constexpr std::size_t to_num_channels(texture_format format) {
        switch(format) {
            using enum texture_format;
        case red:  return 1;
        case rg:   return 2;
        case rgb:  return 3;
        case rgba: return 4;
        }

        throw std::runtime_error{std::format("to_num_channels: unrecognized value of texture_format {}", to_gl_enum(format))};
    }

    struct default_texture_2d_parameter_setter {
        void operator()() {
            gl_function{glGenerateMipmap}(GL_TEXTURE_2D);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        };
    };

    struct texture_2d_configurator {
        constexpr static auto flavour{GL_TEXTURE_2D};
        using value_type = GLubyte;

        image_view data;
        colour_space_flavour colour_space;
        std::function<void()> parameter_setter{};
        optional_label label{};
    };

    struct texture_lifecycle_events {
        using configurator = texture_2d_configurator;

        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenTextures}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteTextures}(N, indices.data()); }

        static void bind(const resource_handle& h) { gl_function{glBindTexture}(GL_TEXTURE_2D, h.index()); }

        static void configure(const resource_handle& h, const configurator& config);
    };
  
    class texture_2d : public generic_resource<num_resources{1}, texture_lifecycle_events> {
    public:
        using base_type         = generic_resource<num_resources{1}, texture_lifecycle_events>;
        using configurator_type = base_type::configurator_type;
        using value_type        = texture_2d_configurator::value_type;

        texture_2d(const configurator_type& textureConfig)
            : base_type{{textureConfig}}
        {}

        friend unique_image extract_image(const texture_2d& tex2d, texture_format format, alignment rowAlignment) {
            base_type::do_bind(tex2d);
            const GLint width{extract_texture_2d_param(GL_TEXTURE_WIDTH)}, height{extract_texture_2d_param(GL_TEXTURE_HEIGHT)};
            const colour_channels numChannels{to_num_channels(format)};

            const auto size{padded_row_size(width, numChannels, rowAlignment) * height};
            std::vector<value_type> texture(size);
            gl_function{glPixelStorei}(GL_PACK_ALIGNMENT, static_cast<int>(rowAlignment.raw_value()));
            gl_function{glGetTexImage}(GL_TEXTURE_2D, 0, to_gl_enum(format), to_gl_enum(to_gl_type_specifier_v<value_type>), texture.data());
            return {texture, static_cast<std::size_t>(width), static_cast<std::size_t>(height), numChannels, rowAlignment};
        }

        /*friend void bind_for_rendering(const texture_object& texObj) {
            [&] <std::size_t... Is>(std::index_sequence<Is...>){
                (texture_object::activate_and_bind<Is>(texObj), ...);
            }(std::make_index_sequence<N>{});
        }*/

    private:
        /*template<std::size_t I>
        static void activate_and_bind(const texture_object& texObj) {
            gl_function{glActiveTexture}(GL_TEXTURE0 + I);
            base_type::do_bind(texObj, index<I>{});
        }*/

        [[nodiscard]]
        static GLint extract_texture_2d_param(GLenum paramName) {
            GLint param{};
            gl_function{glGetTexLevelParameteriv}(GL_TEXTURE_2D, 0, paramName, &param);
            return param;
        }
    };
}
