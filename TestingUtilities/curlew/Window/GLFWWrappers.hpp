////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "RenderingSetup.hpp"

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"
#include "avocet/Core/RenderArea/Viewport.hpp"
#include "avocet/OpenGL/Debugging/Errors.hpp"
#include "avocet/OpenGL/EnrichedContext/CapableContext.hpp"

#include <string>

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

    struct window_config {
        using decorator_type = std::function<void(const agl::context&, const agl::decorator_data)>;

        avocet::discrete_extent dimensions{.width{800}, .height{600}};
        std::string name{};
        window_hiding_mode hiding{window_hiding_mode::off};
        agl::debugging_mode debug_mode{agl::debugging_mode::dynamic};
        decorator_type prologue{},
                       epilogue{agl::standard_error_checker{agl::num_messages{10}, agl::default_debug_info_processor{}}};
        agl::attempt_to_compensate_for_driver_bugs compensate{agl::attempt_to_compensate_for_driver_bugs::yes};
        num_samples samples{1};
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

    class window;

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
        window create_window(const window_config& config);

        [[nodiscard]]
        const rendering_setup& get_rendering_setup() const noexcept { return m_RenderingSetup; }
    };

    class [[nodiscard]] window_resource {
        friend window;

        GLFWwindow& m_Window;

        window_resource(const window_config& config, const agl::opengl_version& version);
    public:

        window_resource(const window_resource&) = delete;

        window_resource& operator=(const window_resource&) = delete;

        ~window_resource();

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window; }

        [[nodiscard]]
        avocet::discrete_extent get_framebuffer_extent() const;
    };

    class [[nodiscard]] window {
        friend glfw_manager;

        window_resource m_Window;
        agl::capable_context m_Context;

        window(const window_config& config, const agl::opengl_version& version);
    public:

        window(const window&) = delete;

        window& operator=(const window&) = delete;

        ~window() = default;

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window.get(); }

        [[nodiscard]] const agl::capable_context& context() const noexcept { return m_Context; }

        void update_viewport(const avocet::viewport& vp);

        [[nodiscard]]
        avocet::discrete_extent get_framebuffer_extent() const {
            return m_Window.get_framebuffer_extent();
        }
    };
}
