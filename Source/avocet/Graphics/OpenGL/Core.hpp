////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Errors.hpp"

#include <concepts>
#include <stdexcept>

namespace avocet::opengl {
    enum class debugging_mode { none = 0, basic, advanced };

    [[nodiscard]]
    constexpr debugging_mode get_debugging_mode() noexcept {
        #if defined(GL_ADVANCED_DEBUGGING)
            return debugging_mode::advanced;
        #elif defined(GL_BASIC_DEBUGGING)
            return debugging_mode::basic;
        #elif defined(NDEBUG)
            // release build
            return debugging_mode::none;
        #elif defined(__APPLE__)
            return debugging_mode::basic;
        #else
            return debugging_mode::basic;
        #endif
    }

    namespace impl {
        template<class Fn, class... Args>
            requires std::invocable<Fn, Args...> && std::is_void_v<std::invoke_result_t<Fn, Args...>>
        void invoke_gl_fn_unsafe(Fn fn, const Args&... args) {
            fn(args...);

            if constexpr(get_debugging_mode() == debugging_mode::basic)
                check_for_errors();
        }

        template<class Fn, class... Args>
            requires std::invocable<Fn, Args...>
        std::invoke_result_t<Fn, Args...> invoke_gl_fn_unsafe(Fn fn, const Args&... args) {
            auto ret{fn(args...)};

            if constexpr(get_debugging_mode() == debugging_mode::basic)
                check_for_errors();

            return ret;
        }
    }

    template<class Fn, class... Args>
        requires std::invocable<Fn, Args...>
    std::invoke_result_t<Fn, Args...> invoke_gl_fn(Fn fn, const Args&... args) {
        if(!fn) throw std::runtime_error{"Null OpenGL function pointer"};
        return impl::invoke_gl_fn_unsafe(fn, args...);
    }
}