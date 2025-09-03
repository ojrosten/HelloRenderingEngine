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

    template<class, debugging_mode Mode = inferred_debugging_mode()> class gl_function;

    template<class R, class... Args>
    using function_pointer_type = R(*)(Args...);

    template<class R, class... Args>
    using glad_ctx_ptr_to_mem_fn_ptr_type = function_pointer_type<R, Args...> GladGLContext::*;

    template<class R, class... Args, debugging_mode Mode>
    class [[nodiscard]] gl_function<R(Args...), Mode> {
    public:
        using pointer_to_member_type = glad_ctx_ptr_to_mem_fn_ptr_type<R, Args...>;

        constexpr static num_messages max_reported_messages{10};

        gl_function(pointer_to_member_type pMember)
            : m_Fn{pMember}
        {}

        gl_function(unchecked_debug_output_t, pointer_to_member_type pMember)
            : gl_function{pMember}
        {
            static_assert(Mode == debugging_mode::none);
        }

        gl_function(nullptr_t) = delete;

        gl_function(unchecked_debug_output_t, nullptr_t) = delete;

        [[nodiscard]]
        R operator()(const GladGLContext& ctx, Args... args, std::source_location loc = std::source_location::current()) const {

            const auto ret{validate(ctx, loc)(args...)};
            check_for_errors(ctx, loc);
            return ret;
        }

        void operator()(const GladGLContext& ctx, Args... args, std::source_location loc = std::source_location::current()) const
            requires std::is_void_v<R>
        {
            validate(ctx, loc)(args...);
            check_for_errors(ctx, loc);
        }
    private:
        pointer_to_member_type m_Fn;

        function_pointer_type<R, Args...> validate(const GladGLContext& ctx, std::source_location loc) const {
            auto fptr{ctx.*m_Fn};
            return fptr ? fptr : throw std::runtime_error{std::format("gl_function: attempting to use a nullptr coming via {}", to_string(loc))};
        }

        static void check_for_errors(const GladGLContext& ctx, std::source_location loc) {
            if constexpr(Mode != debugging_mode::none) {
                if(debug_output_supported(ctx))
                    check_for_advanced_errors(ctx, max_reported_messages, loc);
                else
                    check_for_basic_errors(ctx, max_reported_messages, loc);
            }
        }
    };

    template<class R, class... Args>
    gl_function(glad_ctx_ptr_to_mem_fn_ptr_type<R, Args...>) -> gl_function<R(Args...)>;

    template<class R, class...Args>
    gl_function(unchecked_debug_output_t, glad_ctx_ptr_to_mem_fn_ptr_type<R, Args...>) -> gl_function<R(Args...), debugging_mode::none>;
}