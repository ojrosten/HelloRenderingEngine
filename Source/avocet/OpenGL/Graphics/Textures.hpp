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
    enum class colour_space_flavour : bool { linear, srgb };

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
    constexpr colour_channels to_num_channels(texture_format format) {
        switch(format) {
            using enum texture_format;
        case red:  return colour_channels{1};
        case rg:   return colour_channels{2};
        case rgb:  return colour_channels{3};
        case rgba: return colour_channels{4};
        }

        throw std::runtime_error{std::format("to_num_channels: unrecognized value of texture_format {}", to_gl_enum(format))};
    }

    [[nodiscard]]
    constexpr texture_format to_texture_format(colour_channels numChannels) {
        switch(numChannels.raw_value()) {
            using enum texture_format;
        case 1: return red;
        case 2: return rg;
        case 3: return rgb;
        case 4: return rgba;
        }

        throw std::runtime_error{std::format("to_texture_format: {} colour channels requested but it must be in the range [1,4]", numChannels)};
    }

    struct texture_2d_configurator {
        using value_type = GLubyte;

        image_view data_view;
        colour_space_flavour colour_space;
        optional_label label{};
    };

    struct texture_2d_lifecycle_events {
        using configurator = texture_2d_configurator;

        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { }

        static void bind(const resource_handle& h) { }

        static void configure(const resource_handle& h, const configurator& config);
    };

    class texture_2d : public generic_resource<num_resources{1}, texture_2d_lifecycle_events> {
    public:
        using base_type         = generic_resource<num_resources{1}, texture_2d_lifecycle_events> ;
        using configurator_type = base_type::configurator_type;
        using value_type        = configurator_type::value_type;

        explicit texture_2d(const configurator_type& textureConfig)
            : base_type{{textureConfig}}
        {}

        friend unique_image extract_data(const texture_2d& tex2d, texture_format format, alignment rowAlignment);
    };
}
