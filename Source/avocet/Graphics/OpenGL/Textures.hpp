////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/Core/Images.hpp"

#include "avocet/Graphics/OpenGL/Labels.hpp"
#include "avocet/Graphics/OpenGL/Resources.hpp"
#include "avocet/Utilities/OpenGL/TypeTraits.hpp"

#include <array>
#include <filesystem>
#include <ranges>
#include <span>
#include <vector>

namespace avocet::opengl {
    enum class texture_flavour : GLenum {
        texture_2d = GL_TEXTURE_2D
    };

    void load_to_texture(const image_configuration& config, texture_flavour textureFlavour);

    struct common_texture_lifecycle_events {
        constexpr static auto identifier{object_identifier::texture};

        struct configurator {
            image_configuration image_config;
            optional_label label;
        };

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenTextures}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteTextures}(N, indices.data()); }
    };

    template<texture_flavour Flavour>
    struct texture_lifecycle_events : common_texture_lifecycle_events {
        constexpr static auto flavour{Flavour};

        static void bind(const resource_handle& h) { gl_function{glBindTexture}(to_gl_enum(Flavour), h.index()); }

        static void configure(const resource_handle& h, const configurator& config) {
            add_label(identifier, h, config.label);
            load_to_texture(config.image_config, flavour);
        }
    };

    template<num_resources NumResources, texture_flavour Flavour>
    class texture_object : public generic_resource<NumResources, texture_lifecycle_events<Flavour>> {
    public:
        using base_type         = generic_resource<NumResources, texture_lifecycle_events<Flavour>>;
        using configurator_type = base_type::configurator_type;
        constexpr static auto N{base_type::N};

        texture_object(const std::array<configurator_type, N>& textureConfig)
            : base_type{textureConfig}
        {}

        friend void bind_for_rendering(const texture_object& texObj) {
            [&] <std::size_t... Is>(std::index_sequence<Is...>){
                (texture_object::activate_and_bind<Is>(texObj), ...);
            }(std::make_index_sequence<N>{});
        }

    private:
        template<std::size_t I>
        static void activate_and_bind(const texture_object& texObj) {
            gl_function{glActiveTexture}(GL_TEXTURE0 + I);
            base_type::do_bind(texObj, index<I>{});
        }
    };
}