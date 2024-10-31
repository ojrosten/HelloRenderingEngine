////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Debugging/OpenGL/Errors.hpp"

#include <concepts>
#include <format>
#include <stdexcept>

namespace avocet::opengl {
    template<class, debugging_mode Mode=requested_debugging_mode()> class gl_function;

    struct debug_output_unchecked_t {};

    inline constexpr debug_output_unchecked_t debug_output_unchecked{};

    template<class R, class... Args>
    class [[nodiscard]] gl_function<R(Args...), debugging_mode::ignored> {
    public:
        using function_pointer_type = R(*)(Args...);

        gl_function(debug_output_unchecked_t, function_pointer_type f, std::source_location loc)
            : m_Fn{validate(f, loc)}
        {}

        [[nodiscard]]
        R operator()(Args... args) const { return m_Fn(args...); }
    private:
        function_pointer_type m_Fn;

        [[nodiscard]]
        static function_pointer_type validate(function_pointer_type f, std::source_location loc) {
            return f ? f : throw std::runtime_error{std::format("gl_function: attempting to construct with a nullptr coming via {}", to_string(loc))};
        }
    };

    template<class R, class... Args, debugging_mode Mode>
        requires (Mode != debugging_mode::ignored)
    class [[nodiscard]] gl_function<R(Args...), Mode> {
    public:
        using function_pointer_type = R(*)(Args...);

        gl_function(function_pointer_type f, std::source_location loc = std::source_location::current())
            : m_Fn{debug_output_unchecked, f, loc}
        {}

        [[nodiscard]]
        R operator()(Args... args, std::source_location loc = std::source_location::current()) const {
            const auto ret{m_Fn(args...)};
            check_for_errors(loc);
            return ret;
        }

        void operator()(Args... args, std::source_location loc = std::source_location::current()) const
            requires std::is_void_v<R>
        {
            m_Fn(args...);
            check_for_errors(loc);
        }
    private:
        gl_function<R(Args...), debugging_mode::ignored> m_Fn;
    };

    template<class R, class... Args>
    gl_function(R(*)(Args...)) -> gl_function<R(Args...)>;

    template<class R, class... Args>
    gl_function(debug_output_unchecked_t, R(*)(Args...), std::source_location) -> gl_function<R(Args...), debugging_mode::ignored>;
}