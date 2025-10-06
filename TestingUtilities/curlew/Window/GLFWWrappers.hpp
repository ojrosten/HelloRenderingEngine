////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "RenderingSetup.hpp"

#include "avocet/Core/Preprocessor/PreprocessorDefs.hpp"
#include "glad/gl.h"

#include <string>

struct GLFWwindow;

namespace curlew {
    enum class window_hiding_mode : bool { off, on };

    struct window_config {
        std::size_t width{800}, height{600};
        std::string name{};
        window_hiding_mode hiding{window_hiding_mode::off};
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
        rendering_setup attempt_to_find_rendering_setup(const avocet::opengl::opengl_version referenceVersion) const;
      
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

        window_resource(const window_config& config, const avocet::opengl::opengl_version& version);
    public:

        window_resource(const window_resource&) = delete;

        window_resource& operator=(const window_resource&) = delete;

        ~window_resource();

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window; }
    };

    class [[nodiscard]] window {
        friend glfw_manager;

        window_resource m_Window;
        GladGLContext m_GLContext;

        window(const window_config& config, const avocet::opengl::opengl_version& version);
    public:

        window(const window&) = delete;

        window& operator=(const window&) = delete;

        ~window();

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window.get(); }
        [[nodiscard]] GladGLContext& gl_context() noexcept { return m_GLContext; }
        
        // Context management
        void make_current();
        [[nodiscard]] bool is_current() const noexcept;
        
        // Window operations with automatic context management
        void swap_buffers();
        [[nodiscard]] bool should_close() const noexcept;
        void set_should_close(bool value) noexcept;
    };

    // RAII Context Guard for temporary context switching
    class [[nodiscard]] context_guard {
        GLFWwindow* m_previous_glfw_window;
        GladGLContext* m_previous_glad_context;
        
    public:
        explicit context_guard(window& new_window);
        ~context_guard();
        
        context_guard(const context_guard&) = delete;
        context_guard& operator=(const context_guard&) = delete;
    };

    // Global context management functions
    [[nodiscard]] GladGLContext* get_current_gl_context();
    [[nodiscard]] window* get_current_window();
    [[nodiscard]] bool has_current_context() noexcept;
}
