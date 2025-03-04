////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "curlew/Window/GLFWWrappers.hpp"

#include "avocet/Debugging/OpenGL/DebugMode.hpp"
#include "avocet/Graphics/OpenGL/GLFunction.hpp"
#include "avocet/Utilities/OpenGL/Version.hpp"

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include <iostream>
#include <format>

namespace curlew {
    namespace {
        namespace agl = avocet::opengl;

        void errorCallback(int error, const char* description) {
            std::cerr << std::format("Error - {}: {}\n", error, description);
        }

        void set_debug_context(const avocet::opengl::opengl_version& version) {
            const auto mode{agl::inferred_debugging_mode()};
	          if constexpr((mode == agl::debugging_mode::dynamic) || (mode == agl::debugging_mode::advanced)) {
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
            agl::gl_function{agl::unchecked_debug_output, glGetIntegerv}(GL_CONTEXT_FLAGS, &flags);
            if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                agl::gl_function{agl::unchecked_debug_output, glEnable}(GL_DEBUG_OUTPUT);
                agl::gl_function{agl::unchecked_debug_output, glEnable}(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                agl::gl_function{agl::unchecked_debug_output, glDebugMessageControl}(
                    GL_DONT_CARE,
                    GL_DONT_CARE,
                    GL_DONT_CARE,
                    0,
                    nullptr,
                    GL_TRUE);
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
        : m_OpenGLVersion{do_find_rendering_setup().version}
    {}

    [[nodiscard]]
    rendering_setup glfw_manager::attempt_to_find_rendering_setup(const agl::opengl_version referenceVersion) const {
        auto w{window({.hiding{window_hiding_mode::on}}, referenceVersion)};
        return { agl::get_opengl_version(), agl::get_renderer()};
    }

    [[nodiscard]]
    rendering_setup glfw_manager::do_find_rendering_setup() const {
      const auto setup{attempt_to_find_rendering_setup(agl::opengl_version{})};
      if((setup.version != agl::opengl_version{}) || avocet::is_apple())
          return setup;

      // Assume we only get here if the version is 4.6
      // If this ever fails in practice, it can be fixed.
      return attempt_to_find_rendering_setup(agl::opengl_version{.major{4}, .minor{6}});
    }

    [[nodiscard]]
    rendering_setup glfw_manager::find_rendering_setup() {
        static rendering_setup setup{glfw_manager{}.do_find_rendering_setup()};
        return setup;
    }

    window glfw_manager::create_window(const window_config& config) { return window{config, m_OpenGLVersion}; }

    window_resource::window_resource(const window_config& config, const agl::opengl_version& version) : m_Window{make_window(config, version)} {}

    window_resource::~window_resource() { glfwDestroyWindow(&m_Window); }

    window::window(const window_config& config, const agl::opengl_version& version) : m_Window{config, version} {
        glfwMakeContextCurrent(&m_Window.get());

        if(!gladLoadGL(glfwGetProcAddress))
            throw std::runtime_error{"Failed to initialize GLAD"};

        init_debug();
    }
}
