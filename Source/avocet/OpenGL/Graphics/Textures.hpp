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

    struct texture_2d_configuration {
        constexpr static auto flavour{GL_TEXTURE_2D};
        using value_type = GLubyte;

        image_view data;
        colour_space_flavour colour_space;
        std::function<void()> parameter_setter{};
        optional_label label{};
    };

    struct texture_lifecycle_events {
        using configuration = texture_2d_configuration;

        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenTextures}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteTextures}(N, indices.data()); }

        static void bind(const resource_handle& h) { gl_function{glBindTexture}(GL_TEXTURE_2D, h.index()); }

        static void configure(const resource_handle& h, const configuration& config);
    };

    class texture_2d : public generic_resource<num_resources{1}, texture_lifecycle_events> {
    public:
        using base_type          = generic_resource<num_resources{1}, texture_lifecycle_events>;
        using configuration_type = base_type::configuration_type;
        using value_type         = texture_2d_configuration::value_type;

        texture_2d(const configuration_type& textureConfig)
            : base_type{{textureConfig}}
        {}

        [[nodiscard]]
        friend image extract_image(const texture_2d& texObj, texture_format format) {
            base_type::do_bind(texObj);
            const GLint width{extract_texture_2d_param(GL_TEXTURE_WIDTH)}, height{extract_texture_2d_param(GL_TEXTURE_HEIGHT)};
            const auto numChannels{to_num_channels(format)};

            const auto size{width * numChannels * height};
            std::vector<value_type> texture(size);
            gl_function{glGetTexImage}(GL_TEXTURE_2D, 0, to_gl_enum(format), to_gl_enum(to_gl_type_specifier_v<value_type>), texture.data());
            return {texture, static_cast<std::size_t>(width), static_cast<std::size_t>(height), image_channels{numChannels}, alignment{1}};
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

        /*[[nodiscard]]
        static GLint extract_num_channels() {
            const texture_internal_format internalFormat{extract_texture_2d_param(GL_TEXTURE_INTERNAL_FORMAT)};
            switch(internalFormat)
            {
            using enum texture_internal_format;
            case red:
                return 1;
            case rg:
                return 2;
            case rgb:
                return 3;
            case srgb:
                return 3;
            case rgba:
                return 4;
            case srgba:
                return 4;
            }

            throw std::runtime_error{std::format("texture_2d: internal format {} not currently supported when extracting textures", static_cast<GLint>(internalFormat))};
        }

        [[nodiscard]]
        static GLint get_integer(GLenum name) {
            GLint param{};
            gl_function{glGetIntegerv}(name, &param);
            return param;
        }

        [[nodiscard]]
        static GLint aligned_width(texture_format format) {
            const auto requestedChannels{static_cast<GLint>(to_num_channels(format))};
            const GLint width{extract_texture_2d_param(GL_TEXTURE_WIDTH) * requestedChannels};
 
            if(requestedChannels != extract_num_channels())
                return width;

            const auto alignment{get_integer(GL_PACK_ALIGNMENT)};
            const auto excess{width % alignment};

            switch(alignment) {
            case 1:
                return width;
            case 2:
            case 3:
            case 4:
            {
                
                return excess ? width - excess + alignment : width;
            }
            default:
                throw std::logic_error{std::format("Unexpected value for GL_PACK_ALIGNMENT, {}", alignment)};
            }
        }*/
    };
}