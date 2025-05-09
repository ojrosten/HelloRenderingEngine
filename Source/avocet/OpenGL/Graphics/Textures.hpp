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

#include "sequoia/FileSystem/FileSystem.hpp"

#include <array>
#include <filesystem>
#include <functional>
#include <ranges>
#include <span>
#include <vector>

namespace avocet::opengl {
    enum class colour_space_flavour : int { linear, srgb };

    class image_configuration
    {
        std::filesystem::path m_File{};
        flip_vertically m_Flip{};
        colour_space_flavour m_ColourSpace{colour_space_flavour::srgb};
    public:
        image_configuration(std::filesystem::path textureFile, flip_vertically flip, colour_space_flavour colourSpace)
            : m_File{std::move(textureFile)}
            , m_Flip{flip}
            , m_ColourSpace{colourSpace}
        {}

        [[nodiscard]]
        const std::filesystem::path& file() const noexcept { return m_File; }

        [[nodiscard]]
        flip_vertically flipped() const noexcept { return  m_Flip; }

        [[nodiscard]]
        colour_space_flavour colour_space() const noexcept { return m_ColourSpace; }


        [[nodiscard]]
        friend bool operator==(const image_configuration&, const image_configuration&) = default;
    };

    struct default_texture_2d_parameter_setter {
        void operator()() {
            gl_function{glGenerateMipmap}(GL_TEXTURE_2D);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            gl_function{glTexParameteri}(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        };
    };

    struct texture_2d_configuration {
        constexpr static auto flavour{GL_TEXTURE_2D};

        image_configuration image_config;
        std::function<void()> parameter_setter{default_texture_2d_parameter_setter{}};
        optional_label label{sequoia::back(image_config.file()).generic_string()};
    };

    void load_texture_2d_to_gpu(const texture_2d_configuration& config);

    struct texture_lifecycle_events {
        using configuration = texture_2d_configuration;

        constexpr static auto identifier{object_identifier::texture};

        template<std::size_t N>
        static void generate(raw_indices<N>& indices) { gl_function{glGenTextures}(N, indices.data()); }

        template<std::size_t N>
        static void destroy(const raw_indices<N>& indices) { gl_function{glDeleteTextures}(N, indices.data()); }

        static void bind(const resource_handle& h) { gl_function{glBindTexture}(GL_TEXTURE_2D, h.index()); }

        static void configure(const resource_handle& h, const configuration& config) {
            add_label(identifier, h, config.label);
            load_texture_2d_to_gpu(config);
        }
    };

    template<num_resources NumResources>
    class texture_object : public generic_resource<NumResources, texture_lifecycle_events> {
    public:
        using base_type         = generic_resource<NumResources, texture_lifecycle_events>;
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