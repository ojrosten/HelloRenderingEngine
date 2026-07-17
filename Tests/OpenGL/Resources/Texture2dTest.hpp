////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "Texture2dTestingUtilities.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Context/GLGetters.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    struct texture_data {
        image_data image;
        opengl::sampling_decoding decoding;
        opengl::optional_label label{};
    };

    class texture_2d_test final : public curlew::common_graphics_move_only_test
    {
    public:
        using parallelizable_type = std::false_type;

        using curlew::common_graphics_move_only_test::common_graphics_move_only_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        using opt_data = std::optional<avocet::testing::image_data>;

        void check_semantics_via_texture_data(const reporter& description, const curlew::opengl_window& w, const texture_data& first, const texture_data& second) {
            opengl::texture_2d tex2d{w.context(), opengl::texture_2d_configurator{.common_config{.decoding{first.decoding},  .parameter_setter{}, .label{ first.label}}, .data_view{image_view_over_data(first.image)}}};
            const opengl::resource_handle handle{checked_conversion_to<GLuint>(opengl::get(w.context(), opengl::int_names::texture_binding_2d))};

            do_check_semantics_via_texture_data(description, w, std::move(tex2d), first, second);
            check_gpu_cleanup(w.context(), &GladGLContext::IsTexture, handle);
        }

        void do_check_semantics_via_texture_data(const reporter& description, const curlew::opengl_window& w, opengl::texture_2d&& tex2d, const texture_data& first, const texture_data& second) {
            check_semantics(
                description,
                std::move(tex2d),
                opengl::texture_2d{w.context(), opengl::texture_2d_configurator{.common_config{.decoding{second.decoding}, .parameter_setter{}, .label{second.label}}, .data_view{image_view_over_data(second.image)}}},
                opt_data{ first.image},
                opt_data{second.image},
                opt_data{},
                opt_data{ first.image}
            );
        }

        void check_semantics_via_texture_data(const reporter& description, const curlew::opengl_window& w, const texture_data& sent1, const image_data& extracted1, const texture_data& sent2, const image_data& extracted2) {
            opengl::texture_2d tex2d{opengl::texture_2d{w.context(), opengl::texture_2d_configurator{.common_config{.decoding{sent1.decoding}, .parameter_setter{}, .label{sent1.label}}, .data_view{image_view_over_data(sent1.image)}}}};
            const opengl::resource_handle handle{checked_conversion_to<GLuint>(opengl::get(w.context(), opengl::int_names::texture_binding_2d))};

            do_check_semantics_via_texture_data(description, w, std::move(tex2d), extracted1, sent2, extracted2);
            check_gpu_cleanup(w.context(), &GladGLContext::IsTexture, handle);
        }

        void do_check_semantics_via_texture_data(const reporter& description, const curlew::opengl_window& w, opengl::texture_2d&& tex2d, const image_data& extracted1, const texture_data& sent2, const image_data& extracted2) {
            check_semantics(
                description,
                std::move(tex2d),
                opengl::texture_2d{w.context(), opengl::texture_2d_configurator{.common_config{.decoding{sent2.decoding}, .parameter_setter{}, .label{sent2.label}}, .data_view{image_view_over_data(sent2.image)}}},
                opt_data{extracted1},
                opt_data{extracted2},
                opt_data{},
                opt_data{extracted1}
            );
        }

        [[nodiscard]]
        static image_view image_view_over_data(const image_data& im) {
            return {im.data, im.extent, im.num_channels, im.row_alignment};
        }
    };
}
