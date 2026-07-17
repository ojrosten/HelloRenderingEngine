////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "FramebufferTrackingFreeTest.hpp"

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

        auto creator{
            [](const resourceful_context& ctx) {
                constexpr discrete_extent fbExtent{.width{1}, .height{2}};
                return
                    framebuffer_object{
                        ctx,
                        fbo_configurator{.label{}},
                        framebuffer_texture_2d_configurator{
                            .common_config{},
                            .format{texture_format::rgba},
                            .extent{fbExtent}
                        }
                    };
            }
        };

        auto utilizer{
            [](const framebuffer_object& fbo) {
                fbo.bind(texture_unit{1});
            }
        };

        execute_tests("BindFramebuffer", int_names::draw_framebuffer_binding, creator, utilizer);
        execute_tests("BindFramebuffer", int_names::read_framebuffer_binding, creator, utilizer);
    }
}
