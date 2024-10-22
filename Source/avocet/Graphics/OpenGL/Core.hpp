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
#include <tuple>

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
        inline void do_check_for_errors(std::source_location loc) {
            if constexpr(get_debugging_mode() == debugging_mode::basic)
                check_for_errors(loc);
        }

        template<class Fn, class... Args>
            requires std::invocable<Fn, Args...> && std::is_void_v<std::invoke_result_t<Fn, Args...>>
        void invoke_gl_fn_unsafe(Fn fn, const std::tuple<Args...>& args, std::source_location loc) {
            std::apply(fn, args);
            do_check_for_errors(loc);
        }

        template<class Fn, class... Args>
            requires std::invocable<Fn, Args...>
        std::invoke_result_t<Fn, Args...> invoke_gl_fn_unsafe(Fn fn, const std::tuple<Args...>& args, std::source_location loc) {
            auto ret{std::apply(fn, args)};
            do_check_for_errors(loc);

            return ret;
        }
    }

    template<class Fn, class... Args>
        requires std::invocable<Fn, Args...>
    std::invoke_result_t<Fn, Args...> invoke_gl_fn(Fn fn, const std::tuple<Args...>& args, std::source_location loc=std::source_location::current()) {
        if(!fn) throw std::runtime_error{"Null OpenGL function pointer"};
        return impl::invoke_gl_fn_unsafe(fn, args, loc);
    }

    template<class Fn, class Arg>
        requires std::invocable<Fn, Arg>
    std::invoke_result_t<Fn, Arg> invoke_gl_fn(Fn fn, const Arg& arg, std::source_location loc = std::source_location::current()) {
        return invoke_gl_fn(fn, std::tuple{arg}, loc);
    }
}