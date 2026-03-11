////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "avocet/Core/Geometry/PolygonCoordinates.hpp"
#include "avocet/OpenGL/Resources/Framebuffer.hpp"

#include "curlew/TestFramework/GraphicsTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class polygon_free_test final : public curlew::common_graphics_test
    {
    public:
        using curlew::common_graphics_test::common_graphics_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:

        template<
            opengl::gl_floating_point T,
            std::size_t NumVerts,
            dimensionality Dim,
            std::size_t NumTextures,
            discrete_extent Extent
        >
        struct poly_data {
            std::filesystem::path vertex_shader{}, frag_shader{};

            using image_t = std::array<unsigned char, Extent.width * Extent.height>;
            std::array<image_t, NumTextures> images{};

            std::array<unsigned char, Extent.width> bottom_prediction{};
        };

        template<
            opengl::gl_floating_point T,
            std::size_t NumVerts,
            dimensionality Dim,
            discrete_extent Extent
        >
        void test_polys(const opengl::framebuffer_object& fbo);

        template<
            opengl::gl_floating_point T,
            std::size_t NumVerts,
            dimensionality Dim,
            std::size_t NumTextures,
            discrete_extent Extent
        >
        void test_poly(const opengl::framebuffer_object& fbo, const poly_data<T, NumVerts, Dim, NumTextures, Extent>& polyData) {
            test_poly(fbo, polyData, std::make_index_sequence<NumTextures>{});
        }

        template<
            opengl::gl_floating_point T,
            std::size_t NumVerts,
            dimensionality Dim,
            discrete_extent Extent,
            std::size_t... Is
        >
        void test_poly(const opengl::framebuffer_object& fbo, const poly_data<T, NumVerts, Dim, sizeof...(Is), Extent>& polyData, std::index_sequence<Is...>);


        [[nodiscard]]
        std::filesystem::path get_shader_dir() const { return working_materials() / "Shaders"; }

        [[nodiscard]]
        std::filesystem::path get_vertex_shader_dir() const { return get_shader_dir() / "Vertex"; }

        [[nodiscard]]
        std::filesystem::path get_frag_shader_dir() const { return get_shader_dir() / "Fragment" / "General"; }
    };
}
