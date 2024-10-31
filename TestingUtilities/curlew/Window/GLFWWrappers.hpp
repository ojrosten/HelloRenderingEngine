////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Preprocessor/Core/PreprocessorDefs.hpp"

#include <string>

struct GLFWwindow;

namespace curlew {
    class window;

    enum class window_hiding_mode : bool { off, on };

    struct opengl_version {
        std::size_t major{4}, minor{get_minor_version()};

        constexpr static std::size_t get_minor_version() noexcept { return avocet::is_apple() ? 1 : 6; }
    };

    struct window_config {
        std::size_t width{800}, height{600};
        std::string name{};
        window_hiding_mode hiding{window_hiding_mode::off};
        opengl_version version{};
    };

    struct [[nodiscard]] glfw_manager {
        glfw_manager();

        glfw_manager(const glfw_manager&) = delete;

        glfw_manager& operator=(const glfw_manager&) = delete;

        ~glfw_manager();

        window create_window(const window_config& config);
    };

    class [[nodiscard]] window_resource {
        friend window;

        GLFWwindow& m_Window;

        window_resource(const window_config& config);
    public:

        window_resource(const window_resource&) = delete;

        window_resource& operator=(const window_resource&) = delete;

        ~window_resource();

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window; }
    };

    class [[nodiscard]] window {
        friend glfw_manager;

        window_resource m_Window;

        window(const window_config& config);
    public:

        window(const window&) = delete;

        window& operator=(const window&) = delete;

        ~window() = default;

        [[nodiscard]] GLFWwindow& get() noexcept { return m_Window.get(); }
    };
}