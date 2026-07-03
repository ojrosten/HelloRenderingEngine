////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "FramebufferFreeTest.hpp"
#include "avocet/OpenGL/Context/GLGetters.hpp"
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
        auto win{create_default_window({1, 1})};

        const auto handle{execute(win)};
        check_gpu_cleanup(win.context(), &GladGLContext::IsFramebuffer, handle);
    }

    opengl::resource_handle framebuffer_free_test::execute(const curlew::window& win)
    {
        using namespace curlew;
        using namespace opengl;

        constexpr discrete_extent fbExtent{.width{1}, .height{2}};
        framebuffer_object
            fbo{
                win.context(),
                fbo_configurator{.label{}},
                framebuffer_texture_2d_configurator{
                    .common_config{},
                    .format{texture_format::rgba},
                    .extent{fbExtent}
                }
            };

        gl_function{&GladGLContext::ClearColor}(win.context(), 1.f, 128.f/255.f, 128.f/255.f, 1.f);
        gl_function{&GladGLContext::Clear}(win.context(), GL_COLOR_BUFFER_BIT);

        check(
            equivalence,
            "",
            fbo.extract_data(texture_format::rgba, alignment{1}),
            image_data{
                .data{
                    std::vector<unsigned char> {
                        255, 128, 128, 255,
                        255, 128, 128, 255
                    }
                },
                .extent{fbExtent},
                .num_channels{4},
                .row_alignment{1}
            }
        );

        return resource_handle{checked_conversion_to<GLuint>(get(win.context(), int_names::draw_framebuffer_binding))};
    }
}
