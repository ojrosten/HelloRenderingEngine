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

#include "sequoia/FileSystem/FileSystem.hpp"

#include <array>
#include <filesystem>
#include <functional>
#include <ranges>
#include <span>
#include <vector>

namespace avocet::opengl {
    enum class texture_flavour : GLenum {
        texture_2d = GL_TEXTURE_2D
    };

    template<texture_flavour Flavour>
    struct default_texture_parameter_setter {
        void operator()()= delete;
    };


    template<>
    struct default_texture_parameter_setter<texture_flavour::texture_2d> {
        void operator()() {
            gl_function{glGenerateMipmap}(to_gl_enum(texture_flavour::texture_2d));
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        };
    };

    template<texture_flavour Flavour>
    struct texture_configuration {
        constexpr static auto flavour{Flavour};

        image_configuration image_config;
        std::function<void()> parameter_setter{default_texture_parameter_setter<Flavour>{}};
        optional_label label{sequoia::back(image_config.file()).generic_string()};
    };

    void load_to_texture(const image_configuration& config, texture_flavour textureFlavour);

    template<texture_flavour Flavour>
    void load_to_texture(const texture_configuration<Flavour>& config) {
        load_to_texture(config.image_config, Flavour);
        if(config.parameter_setter) config.parameter_setter();
    }

    struct common_texture_lifecycle_events {
        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenTextures}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteTextures}(N, indices.data()); }
    };

    template<texture_flavour Flavour>
    struct texture_lifecycle_events : common_texture_lifecycle_events {
        constexpr static auto flavour{Flavour};

        using configuration = texture_configuration<flavour>;

        static void bind(const resource_handle& h) { gl_function{glBindTexture}(to_gl_enum(Flavour), h.index()); }

        static void configure(const resource_handle& h, const configuration& config) {
            add_label(identifier, h, config.label);
            load_to_texture(config);
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