////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "Examples/PonyPolygons.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"

#include "GLFW/glfw3.h"

#include <iostream>
#include <format>
#include <print>

namespace agl = avocet::opengl;

int main()
{
    try
    {
        curlew::glfw_manager manager{};
        const auto renderingSetup{manager.get_rendering_setup()};
        std::cout << curlew::rendering_setup_summary(renderingSetup);

        const std::vector<agl::message_id> acceptableWarnings{
            [&renderingSetup]() -> std::vector<agl::message_id> {
                if(curlew::is_intel(renderingSetup.renderer))
                    return {agl::message_id{2}};
                else if(curlew::is_nvidia(renderingSetup.renderer))
                    return {agl::message_id{131204}, agl::message_id{131218}};

                return {};
            }()
        };

        auto w{
            manager.create_window(
                {.width {800},
                 .height{800},
                 .name{"Hello Rendering Engine"},
                 .hiding{curlew::window_hiding_mode::off},
                 .debug_mode{agl::debugging_mode::dynamic},
                 .prologue{},
                 .epilogue{agl::standard_error_checker{agl::num_messages{10}, agl::default_debug_info_processor{acceptableWarnings}}},
                 .compensate{agl::attempt_to_compensate_for_driver_bugs::yes},
                 .samples{4}
                }
            )
        };
        const auto& ctx{w.context()};

        agl::testing::pony_polygons ponyPolygons{ctx};

        while(!glfwWindowShouldClose(&w.get())) {
            agl::gl_function{&GladGLContext::ClearColor}(ctx, 0.2f, 0.3f, 0.3f, 1.0f);
            agl::gl_function{&GladGLContext::Clear}(ctx, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            w.update_viewport(avocet::viewport({}, w.get_framebuffer_extent()));

            ponyPolygons.draw();

            glfwSwapBuffers(&w.get());
            glfwPollEvents();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what();
    }
    catch(...)
    {
        std::cerr << "Unrecognized error\n";
    }
}


