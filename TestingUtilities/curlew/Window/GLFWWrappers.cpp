////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/OpenGL/Debugging/DebugMode.hpp"
#include "avocet/OpenGL/Utilities/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Version.hpp"
#include "avocet/OpenGL/Utilities/ContextResolver.hpp"

#include "GLFW/glfw3.h"

#include <iostream>
#include <format>

namespace curlew {
    namespace {
        namespace agl = avocet::opengl;

        // Thread-local storage for current GL context and window
        thread_local GladGLContext* current_gl_context = nullptr;
        thread_local window* current_window = nullptr;

        void errorCallback(int error, const char* description) {
            std::cerr << std::format("Error - {}: {}\n", error, description);
        }

        void set_debug_context(const avocet::opengl::opengl_version& version) {
            const auto mode{agl::inferred_debugging_mode()};
	          if constexpr(mode != agl::debugging_mode::none) {
                if(agl::debug_output_supported(version))
                    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	          }
        }

        [[nodiscard]]
        constexpr int to_int(window_hiding_mode mode) noexcept { return mode == window_hiding_mode::off; }

        [[nodiscard]]
        GLFWwindow& make_window(const window_config& config, const avocet::opengl::opengl_version& version) {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, static_cast<int>(version.major));
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, static_cast<int>(version.minor));
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
            glfwWindowHint(GLFW_VISIBLE, to_int(config.hiding));

            set_debug_context(version);

            auto win{glfwCreateWindow(static_cast<int>(config.width), static_cast<int>(config.height), config.name.data(), nullptr, nullptr)};
            return win ? *win : throw std::runtime_error{"Failed to create GLFW window"};
        }

        void init_debug()
        {
            GLint flags{};
            // Use the current context directly
            if(auto* ctx = current_gl_context) {
                ctx->GetIntegerv(GL_CONTEXT_FLAGS, &flags);
                if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                    if(const auto version{agl::get_opengl_version()}; !agl::debug_output_supported(version))
                        throw std::runtime_error{std::format("init_debug: inconsistency between context flags {} and OpengGL version {}", flags, version)};

                    ctx->Enable(GL_DEBUG_OUTPUT);
                    ctx->Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                    ctx->DebugMessageControl(
                        GL_DONT_CARE,
                        GL_DONT_CARE,
                        GL_DONT_CARE,
                        0,
                        nullptr,
                        GL_TRUE);
                }
            }
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
    {}

    [[nodiscard]]
    rendering_setup glfw_manager::attempt_to_find_rendering_setup(const agl::opengl_version referenceVersion) const {
        try {
            auto w{window({.hiding{window_hiding_mode::on}}, referenceVersion)};
            // Wait a bit to ensure context is fully initialized
            return {agl::get_opengl_version(), agl::get_vendor(), agl::get_renderer()};
        } catch (const std::exception& e) {
            // If we can't create a window or get OpenGL info, return defaults
            return {agl::opengl_version{1, 0}, "Unknown", "Unknown"};
        }
    }

    [[nodiscard]]
    rendering_setup glfw_manager::do_find_rendering_setup() const {
      const auto setup{attempt_to_find_rendering_setup(agl::opengl_version{})};
      if((setup.version != agl::opengl_version{}) || avocet::is_apple())
          return setup;

      // Assume we only get here if we're on windows, in which case
      // the version is 4.6. If this ever fails in practice, it can be fixed.
      return attempt_to_find_rendering_setup(agl::opengl_version{.major{4}, .minor{6}});
    }

    [[nodiscard]]
    rendering_setup glfw_manager::find_rendering_setup() { return glfw_manager{}.get_rendering_setup(); }

    window glfw_manager::create_window(const window_config& config) { return window{config, m_RenderingSetup.version}; }

    window_resource::window_resource(const window_config& config, const agl::opengl_version& version) : m_Window{make_window(config, version)} {}

    window_resource::~window_resource() { glfwDestroyWindow(&m_Window); }

    window::window(const window_config& config, const agl::opengl_version& version) 
        : m_Window{config, version}
        , m_GLContext{}
    {
        glfwMakeContextCurrent(&m_Window.get());

        if(!gladLoadGLContext(&m_GLContext, glfwGetProcAddress))
            throw std::runtime_error{"Failed to initialize GLAD"};

        // Set this as the current context
        current_gl_context = &m_GLContext;
        current_window = this;

        init_debug();
    }

    void window::make_current() {
        glfwMakeContextCurrent(&m_Window.get());
        current_gl_context = &m_GLContext;
        current_window = this;
    }

    [[nodiscard]] bool window::is_current() const noexcept {
        return current_gl_context == &m_GLContext;
    }

    void window::swap_buffers() {
        // Ensure this window is current before swapping
        if (!is_current()) {
            make_current();
        }
        glfwSwapBuffers(&m_Window.get());
    }

    [[nodiscard]] bool window::should_close() const noexcept {
        return glfwWindowShouldClose(&m_Window.get());
    }

    void window::set_should_close(bool value) noexcept {
        glfwSetWindowShouldClose(&m_Window.get(), value ? GLFW_TRUE : GLFW_FALSE);
    }

    window::~window() {
        // If this window's context is currently active, clear it
        if (current_gl_context == &m_GLContext) {
            current_gl_context = nullptr;
            current_window = nullptr;
        }
    }

    // RAII Context Guard implementation
    context_guard::context_guard(window& new_window) 
        : m_previous_glfw_window(glfwGetCurrentContext())
        , m_previous_glad_context(current_gl_context)
    {
        new_window.make_current();
    }

    context_guard::~context_guard() {
        // Restore previous contexts directly - no dangling pointer risk
        glfwMakeContextCurrent(m_previous_glfw_window);
        current_gl_context = m_previous_glad_context;
        
        // Update current_window tracking if we can
        if (m_previous_glad_context) {
            // We had a context before, but we don't know which window it belongs to
            // This is a limitation - current_window might be incorrect after guard
            // But it's safe (no crashes)
            current_window = nullptr; // Conservative: clear it
        } else {
            current_window = nullptr;
        }
    }

    // Global context management functions
    [[nodiscard]] GladGLContext* get_current_gl_context() {
        return current_gl_context;
    }

    [[nodiscard]] window* get_current_window() {
        return current_window;
    }

    [[nodiscard]] bool has_current_context() noexcept {
        return current_gl_context != nullptr;
    }
}
