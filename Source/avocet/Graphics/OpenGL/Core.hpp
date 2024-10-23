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

    inline void do_check_for_errors(std::source_location loc) {
        if constexpr(get_debugging_mode() == debugging_mode::basic)
            check_for_errors(loc);
    }

   /* template<class Fn, class... Args>
        requires std::invocable<Fn, Args...>
    class gl_function {
        Fn m_Fn;
        std::tuple<Args...> m_Args;
        std::source_location m_Loc;
    public:
        gl_function(Fn f, const Args&... args, std::source_location loc = std::source_location::current())
            : m_Fn{f}
            , m_Args{args...}
            , m_Loc{loc}
        {}

        [[nodiscard]]
        std::invoke_result_t<Fn, Args...> operator()() const {
            auto ret{std::apply(m_Fn, m_Args)};
            do_check_for_errors(m_Loc);

            return ret;
        }

        void operator()() const requires std::is_void_v<std::invoke_result_t<Fn, Args...>> {
            std::apply(m_Fn, m_Args);
            do_check_for_errors(m_Loc);
        }
    };

    template<class Fn, class... Args>
    gl_function(Fn, const Args&...) -> gl_function<Fn, Args...>;*/

    template<class Fn, class... Args>
        requires std::invocable<Fn, Args...>
    class [[nodiscard]] gl_function_invoker {
    public:
        using return_type = std::invoke_result_t<Fn, Args...>;

        gl_function_invoker(Fn f, const Args&... args, std::source_location loc = std::source_location::current())
            : m_Result{f(args...)}
        {
            do_check_for_errors(loc);
        }

        [[nodiscard]]
        return_type get() const { return m_Result; }
    private:
        return_type m_Result;
    };

    template<class Fn, class... Args>
        requires std::invocable<Fn, Args...> && std::is_void_v<std::invoke_result_t<Fn, Args...>>
    class gl_function_invoker<Fn, Args...> {
    public:
        gl_function_invoker(Fn f, const Args&... args, std::source_location loc = std::source_location::current())
        {
            f(args...);
            do_check_for_errors(loc);
        }
    };

    template<class Fn, class... Args>
    gl_function_invoker(Fn, const Args&...) -> gl_function_invoker<Fn, Args...>;
}