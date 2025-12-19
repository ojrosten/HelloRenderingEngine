////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "RenderingSetup.hpp"

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"
#include "avocet/OpenGL/Debugging/Errors.hpp"
#include "avocet/OpenGL/EnrichedContext/CapableContext.hpp"

#include <string>

#include "volk.h"

#include "GLFW/glfw3.h"

struct GLFWwindow;

namespace curlew {
    namespace agl = avocet::opengl;

    enum class window_hiding_mode : bool { off, on };

    class num_samples {
        std::size_t m_Num{1};
    public:
        constexpr explicit num_samples(std::size_t num)
            : m_Num{num}
        {
            if(!num)
                throw std::runtime_error{"Number of samples must be > 0"};
        }

        [[nodiscard]]
        constexpr std::size_t value() const noexcept { return m_Num; }

        [[nodiscard]]
        friend constexpr auto operator<=>(const num_samples&, const num_samples&) noexcept = default;
    };

    struct opengl_window_config {
        using decorator_type = std::function<void(const agl::context&, const agl::decorator_data)>;

        std::size_t width{800}, height{600};
        std::string name{};
        window_hiding_mode hiding{window_hiding_mode::off};
        agl::debugging_mode debug_mode{agl::debugging_mode::dynamic};
        decorator_type prologue{},
                       epilogue{agl::standard_error_checker{agl::num_messages{10}, agl::default_debug_info_processor{}}};
        agl::attempt_to_compensate_for_driver_bugs compensate{agl::attempt_to_compensate_for_driver_bugs::yes};
        num_samples samples{1};
    };

    struct product_info {
        std::string   name{};
        std::uint32_t version{VK_MAKE_VERSION(1, 0, 0)};
    };

    struct vulkan_application_info {
        product_info app{}, engine{};
    };

    struct vulkan_create_info {
        vulkan_application_info app_info{};
    };

    struct vulkan_window_config {
        std::size_t width{800}, height{600};
        vulkan_create_info create_info{};
    };

    class glfw_manager;

    class glfw_resource {
        friend glfw_manager;

        glfw_resource();
    public:
        ~glfw_resource();

        glfw_resource(const glfw_resource&) = delete;

        glfw_resource& operator=(const glfw_resource&) = delete;
    };

    class opengl_window;
    class vulkan_window;

    class [[nodiscard]] glfw_manager {
        glfw_resource m_Resource{};
        rendering_setup m_RenderingSetup{};

        [[nodiscard]]
        rendering_setup attempt_to_find_rendering_setup(const agl::opengl_version referenceVersion) const;
      
        [[nodiscard]]
        rendering_setup do_find_rendering_setup() const;
    public:
        glfw_manager();

        glfw_manager(const glfw_manager&) = delete;

        glfw_manager& operator=(const glfw_manager&) = delete;

        [[nodiscard]]
        opengl_window create_window(const opengl_window_config& config);

        [[nodiscard]]
        vulkan_window create_window(const vulkan_window_config& config);

        [[nodiscard]]
        const rendering_setup& get_rendering_setup() const noexcept { return m_RenderingSetup; }
    };

    class [[nodiscard]] window_resource {
        friend opengl_window;
        friend vulkan_window;

        GLFWwindow& m_Window;

        window_resource(const opengl_window_config& config, const agl::opengl_version& version);

        window_resource(const vulkan_window_config& config);
    public:

        window_resource(const window_resource&) = delete;

        window_resource& operator=(const window_resource&) = delete;

        ~window_resource();

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window; }
    };

    class [[nodiscard]] opengl_window {
        friend glfw_manager;

        window_resource m_Window;
        agl::capable_context m_Context;

        opengl_window(const opengl_window_config& config, const agl::opengl_version& version);
    public:

        opengl_window(const opengl_window&) = delete;

        opengl_window& operator=(const opengl_window&) = delete;

        ~opengl_window() = default;

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window.get(); }

        [[nodiscard]] const agl::capable_context& context() const noexcept { return m_Context; }
    };

    class [[nodiscard]] vulkan_instance {
        VkInstance m_Instance;
    public:
        vulkan_instance(const vulkan_window_config& config);

        vulkan_instance(const vulkan_instance&) = delete;

        vulkan_instance& operator=(const vulkan_instance&) = delete;

        ~vulkan_instance();
    };

    class [[nodiscard]] vulkan_window {
        friend glfw_manager;

        window_resource m_Window;
        vulkan_instance m_Instance;

        vulkan_window(const vulkan_window_config& config);
    public:

        vulkan_window(const vulkan_window&) = delete;

        vulkan_window& operator=(const vulkan_window&) = delete;

        ~vulkan_window() = default;

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window.get(); }
    };
}
