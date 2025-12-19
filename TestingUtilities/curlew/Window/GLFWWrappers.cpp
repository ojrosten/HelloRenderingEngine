////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/EnrichedContext/DecoratedContext.hpp"

#include <iostream>
#include <format>

#include "GLFW/glfw3.h"

namespace curlew {
    namespace {
        void errorCallback(int error, const char* description) {
            std::cerr << std::format("Error - {}: {}\n", error, description);
        }

        void set_debug_context(const agl::debugging_mode mode, const agl::opengl_version& version) {
            const bool advancedDebugging{(mode != agl::debugging_mode::off) && agl::debug_output_supported(version)};
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, advancedDebugging);
        }

        [[nodiscard]]
        constexpr int to_int(window_hiding_mode mode) noexcept { return mode == window_hiding_mode::off; }

        [[nodiscard]]
        constexpr int to_int(num_samples samples) { return static_cast<int>(samples.value()); }

        [[nodiscard]]
        GLFWwindow& make_window(const opengl_window_config& config, const agl::opengl_version& version) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(version.major));
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(version.minor));
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_VISIBLE, to_int(config.hiding));
            glfwWindowHint(GLFW_SAMPLES, to_int(config.samples));

            set_debug_context(config.debug_mode, version);

            auto win{glfwCreateWindow(static_cast<int>(config.width), static_cast<int>(config.height), config.name.data(), nullptr, nullptr)};
            return win ? *win : throw std::runtime_error{"Failed to create GLFW OpenGL window"};
        }

        [[nodiscard]]
        GLFWwindow& make_window(const vulkan_window_config& config) {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_VISIBLE, true);

            auto win{glfwCreateWindow(static_cast<int>(config.width), static_cast<int>(config.height), config.create_info.app_info.app.name.data(), nullptr, nullptr)};
            return win ? *win : throw std::runtime_error{"Failed to create GLFW window suitable for Vulkan"};
        }

        [[nodiscard]]
        GladGLContext load_gl_fuctions(window_resource& win, GladGLContext ctx) {
            glfwMakeContextCurrent(&win.get());

            if(!gladLoadGLContext(&ctx, glfwGetProcAddress))
                throw std::runtime_error{"Failed to initialize GLAD"};

            return ctx;
        }

        struct vulkan_extensions {
            std::uint32_t count{};
            const char** names{glfwGetRequiredInstanceExtensions(&count)};
        };

        [[nodiscard]]
        VkInstance make_instance(const vulkan_window_config& config) {
            VkApplicationInfo appInfo{
                .sType{VK_STRUCTURE_TYPE_APPLICATION_INFO},
                .pApplicationName{config.create_info.app_info.app.name.data()},
                .applicationVersion{config.create_info.app_info.app.version},
                .pEngineName{config.create_info.app_info.engine.name.data()},
                .engineVersion{config.create_info.app_info.engine.version},
                .apiVersion{VK_API_VERSION_1_0}
            };
            vulkan_extensions extensions{};
            VkInstanceCreateInfo createInfo{
                .sType{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO},
                .pApplicationInfo{&appInfo},
                .enabledExtensionCount{extensions.count},
                .ppEnabledExtensionNames{extensions.names}
            };

            VkInstance instance{};
            if(auto result{vkCreateInstance(&createInfo, nullptr, &instance)}; result != VK_SUCCESS)
                throw std::runtime_error{std::format("Vulkan instance creation failed with error code {}", static_cast<int>(result))};

            return instance;
        }
    }

    glfw_resource::glfw_resource() {
        glfwSetErrorCallback(errorCallback);

        if(!glfwInit())
            throw std::runtime_error{"Failed to initialize GLFW\n"};
    }

    glfw_resource::~glfw_resource() { glfwTerminate(); }

    glfw_manager::glfw_manager()
        : m_RenderingSetup{do_find_rendering_setup()}
    {
        if(volkInitialize() != VK_SUCCESS) {
            throw std::runtime_error{"Unable to initialize Volk\n"};
        }
    }

    [[nodiscard]]
    rendering_setup glfw_manager::attempt_to_find_rendering_setup(const agl::opengl_version referenceVersion) const {
        auto w{opengl_window({.hiding{window_hiding_mode::on}, .debug_mode{agl::debugging_mode::off}}, referenceVersion)};
        return {agl::get_opengl_version(w.context()), agl::get_vendor(w.context()), agl::get_renderer(w.context())};
    }

    [[nodiscard]]
    rendering_setup glfw_manager::do_find_rendering_setup() const {
        constexpr agl::opengl_version trialVersion{.major{4}, .minor{avocet::is_windows() ? 6 : 1}};

        const auto setup{attempt_to_find_rendering_setup(trialVersion)};
        if(avocet::is_windows() || avocet::is_apple()) {
            if(setup.version != trialVersion)
                throw std::runtime_error{std::format("On {} expected OpenGL version {} but found {}", avocet::is_windows() ? "Windows" : "Apple", trialVersion, setup.version)};
        }
        else if(setup.version == trialVersion) {
            // On some (but not all) linux systems, requesting a trial version of 4.1 will return
            // a version of 4.1, even if higher OpenGL versions are available. However, all such
            // systems I've encountered to date happen to support 4.6. If the latter turns out not
            // to be the case somewhere, than the following will need to be amended in order to
            // divine the actual maximum supported version.
            return attempt_to_find_rendering_setup(agl::opengl_version{.major{4}, .minor{6}});
        }

        return setup;
    }

    opengl_window glfw_manager::create_window(const opengl_window_config& config) { return opengl_window{config, m_RenderingSetup.version}; }

    vulkan_window glfw_manager::create_window(const vulkan_window_config& config) { return vulkan_window{config}; }

    window_resource::window_resource(const opengl_window_config& config, const agl::opengl_version& version) : m_Window{make_window(config, version)} {}

    window_resource::window_resource(const vulkan_window_config& config) : m_Window{make_window(config)} {}

    window_resource::~window_resource() { glfwDestroyWindow(&m_Window); }

    opengl_window::opengl_window(const opengl_window_config& config, const agl::opengl_version& version)
        : m_Window{config, version}
        , m_Context{
            config.debug_mode,
            [&win = m_Window](GladGLContext ctx) { return load_gl_fuctions(win, ctx); },
            config.prologue,
            config.epilogue,
            config.compensate
          }
    {}

    vulkan_window::vulkan_window(const vulkan_window_config& config)
        : m_Window{config}
        , m_Instance{config}
    {}

    vulkan_instance::vulkan_instance(const vulkan_window_config& config)
        : m_Instance{make_instance(config)}
    {
        volkLoadInstanceOnly(m_Instance);
    }

    vulkan_instance::~vulkan_instance() {
        vkDestroyInstance(m_Instance, nullptr);
    }
}
