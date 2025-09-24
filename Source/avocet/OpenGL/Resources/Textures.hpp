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
#include <format>
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
        static void generate(const decorated_context& ctx, raw_indices<N>& indices) { gl_function{&GladGLContext::GenTextures}(ctx, N, indices.data()); }

        template<std::size_t N>
        static void destroy(const decorated_context& ctx, const raw_indices<N>& indices) { gl_function{&GladGLContext::DeleteTextures}(ctx, N, indices.data()); }

        static void bind(const contextual_resource_handle& h) { gl_function{&GladGLContext::BindTexture}(h.context(), GL_TEXTURE_2D, get_index(h)); }

        static void configure(const contextual_resource_handle& h, const configurator& config);
    };

    struct texture_unit {
        std::size_t index{};

        [[nodiscard]]
        GLenum gl_texture_unit() const { return GL_TEXTURE0 + to_gl_int(index); }

        [[nodiscard]]
        friend constexpr auto operator<=>(const texture_unit&, const texture_unit&) noexcept = default;
    };
}

namespace std {
    template<>
    struct formatter<avocet::opengl::texture_unit> {
        constexpr auto parse(auto& ctx) { return ctx.begin(); }

        auto format(avocet::opengl::texture_unit unit, auto& ctx) const {
            return format_to(ctx.out(), "{}", unit.index);
        }
    };
}

namespace avocet::opengl {
    class texture_2d : public generic_resource<num_resources{1}, texture_2d_lifecycle_events> {
    public:
        using base_type         = generic_resource<num_resources{1}, texture_2d_lifecycle_events> ;
        using configurator_type = base_type::configurator_type;
        using value_type        = configurator_type::value_type;

        explicit texture_2d(const decorated_context& ctx, const configurator_type& textureConfig)
            : base_type{ctx, {textureConfig}}
            , m_MaxUnit{get_max_combined_texture_unit(ctx)}
        {}

        [[nodiscard]]
        friend unique_image extract_data(const texture_2d& tex2d, texture_format format, alignment rowAlignment) {
            return do_extract_data(tex2d, format, rowAlignment);
        }

        friend void bind(const texture_2d& tex2d, texture_unit unit) {
            if(unit >= tex2d.m_MaxUnit)
                throw std::runtime_error{std::format("Requested texture unit {} exceeds the maximum indexable {}", unit, tex2d.m_MaxUnit)};

            gl_function{&GladGLContext::ActiveTexture}(tex2d.context(), unit.gl_texture_unit());
            base_type::do_bind(tex2d);
        }
    private:
        texture_unit m_MaxUnit;
        static unique_image do_extract_data(const texture_2d& tex2d, texture_format format, alignment rowAlignment);
 
        [[nodiscard]]
        static texture_unit get_max_combined_texture_unit(const decorated_context& ctx) {
            GLint param{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &param);
            return texture_unit{static_cast<std::size_t>(param)};
        }
    };
}
