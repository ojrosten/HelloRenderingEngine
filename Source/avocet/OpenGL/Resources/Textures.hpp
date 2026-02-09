////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/AssetManagement/Image.hpp"
#include "avocet/Core/Geometry/Extent.hpp"

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

    template<gl_arithmetic T>
    struct raw_texture_2d_configurator {
        texture_format     format;
        discrete_extent    extent;
        std::span<const T> data;
    };

    template<class T>
    inline constexpr bool defines_texture_configuration_v{
           has_configurator_type_v<T>
        && requires (contextual_resource_view crv, const T::configurator & config) {
               T::preliminary_configuration(crv, config);
               { T::to_raw_configurator(config) } -> std::convertible_to<raw_texture_2d_configurator<typename T::configurator::value_type>>;
           }
    };

    struct common_texture_2d_lifecycle_events {
        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(const decorated_context& ctx, raw_indices<N>& indices) { gl_function{&GladGLContext::GenTextures}(ctx, N, indices.data()); }

        template<std::size_t N>
        static void destroy(const decorated_context& ctx, const raw_indices<N>& indices) { gl_function{&GladGLContext::DeleteTextures}(ctx, N, indices.data()); }

        static void bind(contextual_resource_view crv) { gl_function{&GladGLContext::BindTexture}(crv.context(), GL_TEXTURE_2D, get_index(crv)); }
    };


    struct texture_configurator_common {
        using value_type = GLubyte;

        sampling_decoding     decoding;
        std::function<void()> parameter_setter;
        optional_label        label{};
    };

    struct texture_2d_configurator {
        using value_type = texture_configurator_common::value_type;

        texture_configurator_common common_config;
        image_view                  data_view;
    };

    struct texture_2d_lifecycle_events : common_texture_2d_lifecycle_events {
        using configurator = texture_2d_configurator;

        static void configure(contextual_resource_view crv, const configurator& config);
    };

    struct texture_unit {
        std::size_t index{};

        [[nodiscard]]
        GLenum gl_texture_unit() const { return GL_TEXTURE0 + to_gl_int(index); }

        [[nodiscard]]
        friend constexpr auto operator<=>(const texture_unit&, const texture_unit&) noexcept = default;
    };

    class generic_texture_2d : public generic_resource<num_resources{1}, texture_2d_lifecycle_events> {
    public:
        using base_type         = generic_resource<num_resources{1}, texture_2d_lifecycle_events> ;
        using configurator_type = base_type::configurator_type;
        using value_type        = configurator_type::value_type;

        generic_texture_2d(const decorated_context& ctx, const configurator_type& textureConfig)
            : base_type{ctx, {textureConfig}}
        {}

        [[nodiscard]]
        unique_image extract_data(this const generic_texture_2d& self, texture_format format, alignment rowAlignment);

        void bind(this const generic_texture_2d& self, texture_unit unit) {
            gl_function{&GladGLContext::ActiveTexture}(self.context(), unit.gl_texture_unit());
            base_type::do_bind(self);
        }
    protected:
        ~generic_texture_2d() = default;

        generic_texture_2d(generic_texture_2d&&)            noexcept = default;
        generic_texture_2d& operator=(generic_texture_2d&&) noexcept = default;
    };

    class texture_2d : public generic_texture_2d {
    public:
        using generic_texture_2d::generic_texture_2d;
    };
}
