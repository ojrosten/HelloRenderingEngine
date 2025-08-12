////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/AssetManagement/Image.hpp"

#include "avocet/OpenGL/Resources/GenericResource.hpp"
#include "avocet/OpenGL/Resources/Labels.hpp"
#include "avocet/OpenGL/Utilities/TypeTraits.hpp"

#include <array>
#include <filesystem>
#include <functional>
#include <ranges>
#include <span>
#include <vector>

namespace avocet::opengl {
    enum class sampling_decoding : bool { none, srgb };

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

        image_view        data_view;
        sampling_decoding decoding;
        std::function<void()> parameter_setter;
        optional_label    label{};
    };

    struct texture_2d_lifecycle_events {
        using configurator = texture_2d_configurator;

        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenTextures}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteTextures}(N, indices.data()); }

        static void bind(const resource_handle& h) { gl_function{glBindTexture}(GL_TEXTURE_2D, h.index()); }

        static void configure(const resource_handle& h, const configurator& config);
    };

    struct texture_unit {
        std::size_t index{};

        [[nodiscard]]
        GLenum gl_texture_unit() const { return GL_TEXTURE0 + to_gl_int(index); }

        [[nodiscard]]
        friend constexpr auto operator<=>(const texture_unit&, const texture_unit&) noexcept = default;
    };

    class texture_2d : public generic_resource<num_resources{1}, texture_2d_lifecycle_events> {
    public:
        using base_type         = generic_resource<num_resources{1}, texture_2d_lifecycle_events> ;
        using configurator_type = base_type::configurator_type;
        using value_type        = configurator_type::value_type;

        explicit texture_2d(const configurator_type& textureConfig)
            : base_type{{textureConfig}}
        {}

        [[nodiscard]]
        friend unique_image extract_data(const texture_2d& tex2d, texture_format format, alignment rowAlignment) {
            return do_extract_data(tex2d, format, rowAlignment);
        }

        friend void bind(const texture_2d& tex, texture_unit unit) {
            gl_function{glActiveTexture}(unit.gl_texture_unit());
            base_type::do_bind(tex);
        }
    private:
        static unique_image do_extract_data(const texture_2d& tex2d, texture_format format, alignment rowAlignment);
    };
}
