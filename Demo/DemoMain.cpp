////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "Examples/PonyPolygons.hpp"
#include "Examples/FileSystem.hpp"

#include "curlew/Window/GLFWWrappers.hpp"
#include "curlew/Window/RenderingSetup.hpp"
#include "curlew/Window/VulkanInitialization.hpp"

#include "avocet/OpenGL/Resources/ShaderProgram.hpp"
#include "avocet/OpenGL/Geometry/Polygon.hpp"

#include "sequoia/FileSystem/FileSystem.hpp"

#include <iostream>
#include <print>

#include "GLFW/glfw3.h"

namespace agl = avocet::opengl;

int main()
{
    try
    {
        avocet::vulkan::instance_info vulkankInstanceInfo{
            .app_info{.app{.name{"Hello Vulkan Rendering Engine Project"}}},
            .validation_layers{{"VK_LAYER_KHRONOS_validation"}},
            .extensions{VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME}
        };

        curlew::glfw_manager manager{vulkankInstanceInfo};
        const auto renderingSetup{manager.get_rendering_setup()};
        std::cout << curlew::rendering_setup_summary(renderingSetup);

        std::println("Extension Properites");
        for(const auto& p : manager.vulkan_extension_properties()) {
            std::println("{:45}, version {}", p.extensionName.data(), p.specVersion);
        }

        std::println("Layer Properites");
        for(const auto& p : manager.vulkan_layer_properties()) {
            std::println("{:45}, Spec version {}, Impl version {}, {}", p.layerName.data(), p.specVersion, p.implementationVersion, p.description.data());
        }

        const std::vector<agl::message_id> acceptableWarnings{
            [&renderingSetup]() -> std::vector<agl::message_id> {
                if(curlew::is_intel(renderingSetup.renderer))
                    return {agl::message_id{2}};
                else if(curlew::is_nvidia(renderingSetup.renderer))
                    return {agl::message_id{131204}, agl::message_id{131218}};

                return {};
            }()
        };

        auto make_device_extensions{
            []() {
                std::vector<std::string> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
                if constexpr(avocet::is_apple()) {
                    extensions.push_back("VK_KHR_portability_subset");
                }

                return extensions;
            }
        };

        auto vulkanWindow{
            manager.create_window(
                curlew::vulkan_window_config{
                    .width{800},
                    .height{800},
                    .name{"Hello Vulkan Rendering Engine"},
                    .device_config{
                        .selector{curlew::vulkan::device_selector{}},
                        .extensions{make_device_extensions()}
                    },
                    .swap_chain_config{
                        .format_selector{curlew::vulkan::swap_chain_format_selector{}},
                        .present_mode_selector{curlew::vulkan::swap_chain_present_mode_selector{}},
                        .extent_selector{curlew::vulkan::swap_chain_extent_selector{}},
                        .image_usage_flags{vk::ImageUsageFlagBits::eColorAttachment}
                    }
                }
            )
        };

        constexpr avocet::discrete_extent nominalWindowSize{.width{800}, .height{800}};

        auto w{
            manager.create_window(
                {.dimensions{nominalWindowSize},
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

        vulkanWindow.make_renderer(avocet::testing::get_vertex_shader_dir() / "2D" / "FixedColouredTri.vs", avocet::testing::get_fragment_shader_dir() / "General" / "PropagatedColour.fs", avocet::vulkan::frames_in_flight{2});

        while(!glfwWindowShouldClose(&w.get()) || !glfwWindowShouldClose(&vulkanWindow.get())) {
            if(!glfwWindowShouldClose(&w.get())) {
                agl::gl_function{&GladGLContext::ClearColor}(ctx, 0.2f, 0.3f, 0.3f, 1.0f);
                agl::gl_function{&GladGLContext::Clear}(ctx, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

                if(const auto optViewport{avocet::refit(nominalWindowSize, w.get_framebuffer_extent())}; optViewport) {
                    w.update_viewport(optViewport.value());
                    ponyPolygons.draw();
                    glfwSwapBuffers(&w.get());
                }
            }

            if(!glfwWindowShouldClose(&vulkanWindow.get())) {
                vulkanWindow.draw_all();
            }

            glfwPollEvents();
        }

        vulkanWindow.wait_idle();
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


