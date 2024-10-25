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
    enum class debugging_mode { none = 0, basic };

    [[nodiscard]]
    constexpr debugging_mode get_debugging_mode() noexcept {
        #if defined(NDEBUG)
            return debugging_mode::none;
        #else
            return debugging_mode::basic;
        #endif
    }

    template<class Fn, class... Args>
        requires std::invocable<Fn, Args...>
    std::invoke_result_t<Fn, Args...> safe_invoke(Fn f, Args... args) {
        if(!f) throw std::runtime_error{"Null OpenGL function pointer"};
        return f(args...);
    }

    template<class Fn, debugging_mode Mode=get_debugging_mode()>
    class [[nodiscard]] gl_function {
        Fn m_Fn;
        std::source_location m_Loc;
    public:
        gl_function(Fn f, std::source_location loc = std::source_location::current())
            : m_Fn{f}
            , m_Loc{loc}
        {}

        template<class... Args>
            requires std::invocable<Fn, Args...>
        [[nodiscard]]
        std::invoke_result_t<Fn, Args...> operator()(Args... args) const {
            const auto ret{safe_invoke(m_Fn, args...)};
            check_for_errors(m_Loc);

            return ret;
        }

        template<class... Args>
            requires std::invocable<Fn, Args...> && std::is_void_v<std::invoke_result_t<Fn, Args...>>
        void operator()(Args... args) const {
            safe_invoke(m_Fn, args...);
            check_for_errors(m_Loc);
        }
    };

    template<class Fn>
    class [[nodiscard]] gl_function<Fn, debugging_mode::none> {
        Fn m_Fn;
    public:
        gl_function(Fn f) : m_Fn{f} {}

        template<class... Args>
            requires std::invocable<Fn, Args...>
        std::invoke_result_t<Fn, Args...> operator()(Args... args) const { return safe_invoke(m_Fn, args...); }
    };
}