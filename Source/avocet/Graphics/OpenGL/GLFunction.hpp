////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Debugging/OpenGL/Errors.hpp"

#include <concepts>
#include <stdexcept>

namespace avocet::opengl {
    template<class> class gl_function;

    template<class R, class... Args>
    class gl_function<R(Args...)> {
    public:
        using function_pointer_type = R(*)(Args...);

        gl_function(function_pointer_type f, std::source_location loc = std::source_location::current())
            : m_Fn{f ? f : throw std::runtime_error{std::format("gl_function: attempting to construct with a nullptr coming via {}", to_string(loc))}}
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
    };

    template<class R, class... Args>
    gl_function(R(*)(Args...)) -> gl_function<R(Args...)>;
}