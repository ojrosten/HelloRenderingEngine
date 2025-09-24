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

#include <string>

struct GLFWwindow;

namespace curlew {
    namespace agl = avocet::opengl;

    enum class window_hiding_mode : bool { off, on };

    struct window_config {
        using decorator_type = std::function<void(const agl::decorated_context&, const agl::decorator_data)>;

        std::size_t width{800}, height{600};
        std::string name{};
        window_hiding_mode hiding{window_hiding_mode::off};
        agl::debugging_mode debug_mode{agl::debugging_mode::dynamic};
        decorator_type prologue{},
                       epilogue{agl::standard_error_checker{agl::num_messages{10}}};
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

        [[nodiscard]]
        static rendering_setup find_rendering_setup();
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
    };

    class [[nodiscard]] window {
        friend glfw_manager;

        window_resource m_Window;
        agl::decorated_context m_Context{};

        window(const window_config& config, const agl::opengl_version& version);
    public:

        window(const window&) = delete;

        window& operator=(const window&) = delete;

        ~window() = default;

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window.get(); }

        [[nodiscard]] const agl::decorated_context& context() const noexcept { return m_Context; }
    };
}
