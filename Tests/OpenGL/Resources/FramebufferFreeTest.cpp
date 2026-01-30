////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "FramebufferFreeTest.hpp"
#include "avocet/OpenGL/Resources/Framebuffer.hpp"

#include "Core/AssetManagement/ImageTestingUtilities.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path framebuffer_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void framebuffer_free_test::run_tests()
    {
        using namespace curlew;
        using namespace opengl;

        avocet::discrete_extent extent{.width{1}, .height{2}};
        auto w{create_window({.dimensions{extent}, .hiding{window_hiding_mode::on}})};

        framebuffer_object fbo{w.context(), fbo_configurator{.label{}}, framebuffer_texture_2d_configurator{extent}};
        fbo.bind(texture_unit{1});

        agl::gl_function{&GladGLContext::ClearColor}(w.context(), 1.0f, 0.5f, 0.5f, 1.0f);
        agl::gl_function{&GladGLContext::Clear}(w.context(), GL_COLOR_BUFFER_BIT);

        check(
            equivalence,
            "",
            fbo.extract_data(texture_format::rgba, alignment{4}),
            image_data{std::vector<unsigned char>{255, 128, 128, 255, 255, 128, 128, 255}, extent.width, extent.height, colour_channels{4}, alignment{4}}
        );
    }
}
