////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

struct GLFWwindow;

namespace demo {
    class window;

    struct [[nodiscard]] glfw_manager {
        glfw_manager();

        glfw_manager(const glfw_manager&) = delete;

        glfw_manager& operator=(const glfw_manager&) = delete;

        ~glfw_manager();

        window create_window();
    };

    class [[nodiscard]] window {
        friend glfw_manager;

        GLFWwindow& m_Window;

        window();
    public:

        window(const window&) = delete;

        window& operator=(const window&) = delete;

        ~window();

        [[nodiscard]] GLFWwindow& get() { return m_Window; }
    };
}