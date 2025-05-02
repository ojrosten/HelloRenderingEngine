////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/Errors.hpp"

#include <concepts>
#include <format>
#include <stdexcept>

namespace avocet::opengl {
    struct unchecked_debug_output_t {};

    inline constexpr unchecked_debug_output_t unchecked_debug_output{};

    template<class, debugging_mode Mode=inferred_debugging_mode()> class gl_function;

    template<class R, class... Args, debugging_mode Mode>
    class [[nodiscard]] gl_function<R(Args...), Mode> {
    public:
        using function_pointer_type = R(*)(Args...);

        constexpr static num_messages max_reported_messages{10};

        gl_function(function_pointer_type f, std::source_location loc = std::source_location::current())
            : m_Fn{validate(f, loc)}
        {}

        gl_function(unchecked_debug_output_t, function_pointer_type f, std::source_location loc = std::source_location::current())
            : m_Fn{validate(f, loc)}
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
        function_pointer_type m_Fn;

        static function_pointer_type validate(function_pointer_type f, std::source_location loc) {
            return f ? f : throw std::runtime_error{std::format("gl_function: attempting to construct with a nullptr coming via {}", to_string(loc))};
        }

        static void check_for_errors(std::source_location loc) {
            if constexpr(Mode == debugging_mode::basic)
                check_for_basic_errors(max_reported_messages, loc);
            else if constexpr(Mode == debugging_mode::advanced)
                check_for_advanced_errors(max_reported_messages, loc);
            else if constexpr(Mode == debugging_mode::dynamic) {
                if(debug_output_supported())
                    check_for_advanced_errors(max_reported_messages, loc);
                else
                    check_for_basic_errors(max_reported_messages, loc);
            }
        }
    };

    template<class R, class... Args>
    gl_function(R(*)(Args...)) -> gl_function<R(Args...)>;

    template<class R, class...Args>
    gl_function(unchecked_debug_output_t, R(*)(Args...)) -> gl_function<R(Args...), debugging_mode::none>;

    template<class R, class...Args>
    gl_function(unchecked_debug_output_t, R(*)(Args...), std::source_location) -> gl_function<R(Args...), debugging_mode::none>;
}