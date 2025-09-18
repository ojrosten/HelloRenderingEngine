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
	          if constexpr(mode != agl::debugging_mode::off) {
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

        void load_gl_fuctions(window_resource& win, GladGLContext& ctx) {
            glfwMakeContextCurrent(&win.get());

            if(!gladLoadGLContext(&ctx, glfwGetProcAddress))
                throw std::runtime_error{"Failed to initialize GLAD"};
        }

        void init_debug(const agl::decorated_context& ctx)
        {
            GLint flags{};
            agl::gl_function{agl::unchecked_debug_output, &GladGLContext::GetIntegerv}(ctx, GL_CONTEXT_FLAGS, &flags);
            if(flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
                if(const auto version{agl::get_opengl_version(ctx)}; !agl::debug_output_supported(version))
                    throw std::runtime_error{std::format("init_debug: inconsistency between context flags {} and OpengGL version {}", flags, version)};

                agl::gl_function{agl::unchecked_debug_output, &GladGLContext::Enable}(ctx, GL_DEBUG_OUTPUT);
                agl::gl_function{agl::unchecked_debug_output, &GladGLContext::Enable}(ctx, GL_DEBUG_OUTPUT_SYNCHRONOUS);
                agl::gl_function{agl::unchecked_debug_output, &GladGLContext::DebugMessageControl}(
                    ctx,
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
        : m_RenderingSetup{do_find_rendering_setup()}
    {}

    [[nodiscard]]
    rendering_setup glfw_manager::attempt_to_find_rendering_setup(const agl::opengl_version referenceVersion) const {
        auto w{window({.hiding{window_hiding_mode::on}}, referenceVersion)};
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

    [[nodiscard]]
    rendering_setup glfw_manager::find_rendering_setup() { return glfw_manager{}.get_rendering_setup(); }

    window glfw_manager::create_window(const window_config& config) { return window{config, m_RenderingSetup.version}; }

    window_resource::window_resource(const window_config& config, const agl::opengl_version& version) : m_Window{make_window(config, version)} {}

    window_resource::~window_resource() { glfwDestroyWindow(&m_Window); }

    window::window(const window_config& config, const agl::opengl_version& version)
        : m_Window{config, version}
        , m_Context{[&win = m_Window](GladGLContext& ctx) { load_gl_fuctions(win, ctx); }}
    {
        init_debug(m_Context);
    }
}
