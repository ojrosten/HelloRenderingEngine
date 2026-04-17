////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "FramebufferTrackingFreeTest.hpp"
#include "avocet/OpenGL/Resources/Framebuffer.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path framebuffer_tracking_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void framebuffer_tracking_free_test::run_tests()
    {
        using namespace curlew;
        using namespace opengl;

        auto w{create_window({.extent{.width{1}, .height{1}}, .hiding{window_hiding_mode::on}})};

        constexpr discrete_extent fbExtent{.width{1}, .height{2}};
        framebuffer_object
            fbo{
                w.context(),
                fbo_configurator{.label{}},
                framebuffer_texture_2d_configurator{
                    .common_config{},
                    .format{texture_format::rgba},
                    .extent{fbExtent}
                }
            };

        gl_function{&GladGLContext::ClearColor}(w.context(), 1.0, 0.5, 0.5, 1.0);
        gl_function{&GladGLContext::Clear}(w.context(), GL_COLOR_BUFFER_BIT);
        fbo.bind(texture_unit{1});
    }
}
