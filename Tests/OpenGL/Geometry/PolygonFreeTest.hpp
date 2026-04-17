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
            opengl::gl_floating_point CoordsValueType,
            std::size_t NumVerts,
            dimensionality Dim,
            discrete_extent Extent,
            opengl::gl_floating_point... TextureCoordsValueTypes
        >
        struct poly_data {
            std::filesystem::path vertex_shader{}, frag_shader{};

            using image_t = std::array<unsigned char, Extent.width * Extent.height>;
            std::array<image_t, sizeof...(TextureCoordsValueTypes)> images{};

            std::array<unsigned char, Extent.width> bottom_prediction{};
        };

        template<
            opengl::gl_floating_point CoordsValueType,
            std::size_t NumVerts,
            dimensionality Dim,
            opengl::gl_floating_point TextureCoordsValueType,
            discrete_extent Extent
        >
        void test_polys(const opengl::resourceful_context& ctx, const opengl::framebuffer_object& fbo);

        template<
            opengl::gl_floating_point CoordsValueType,
            std::size_t NumVerts,
            dimensionality Dim,
            discrete_extent Extent,
            opengl::gl_floating_point... TextureCoordsValueTypes
        >
        void test_poly(const opengl::resourceful_context& ctx, const opengl::framebuffer_object& fbo, const poly_data<CoordsValueType, NumVerts, Dim, Extent, TextureCoordsValueTypes...>& polyData) {
            test_poly(ctx, fbo, polyData, std::make_index_sequence<sizeof...(TextureCoordsValueTypes)>{});
        }

        template<
            opengl::gl_floating_point CoordsValueType,
            std::size_t NumVerts,
            dimensionality Dim,
            discrete_extent Extent,
            opengl::gl_floating_point... TextureCoordsValueTypes,
            std::size_t... Is
        >
        void test_poly(const opengl::resourceful_context& ctx, const opengl::framebuffer_object& fbo, const poly_data<CoordsValueType, NumVerts, Dim, Extent, TextureCoordsValueTypes...>& polyData, std::index_sequence<Is...>);


        [[nodiscard]]
        std::filesystem::path get_shader_dir() const { return working_materials() / "Shaders"; }

        [[nodiscard]]
        std::filesystem::path get_vertex_shader_dir() const { return get_shader_dir() / "Vertex"; }

        [[nodiscard]]
        std::filesystem::path get_frag_shader_dir() const { return get_shader_dir() / "Fragment" / "General"; }
    };
}
