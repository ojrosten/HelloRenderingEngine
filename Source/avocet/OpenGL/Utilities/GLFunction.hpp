////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/Errors.hpp"

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <format>
#include <stdexcept>

namespace avocet::opengl {

    template<class, debugging_mode Mode=inferred_debugging_mode()> class gl_function;

    template<class R, class... Args>
    using function_pointer_type = R(*)(Args...);

    template<class R, class... Args>
    using glad_ctx_ptr_to_mem_PtrToMem_ptr_type = function_pointer_type<R, Args...> GladGLContext::*;

    template<class R, class... Args, debugging_mode Mode>
    class [[nodiscard]] gl_function<R(Args...), Mode> {
    public:
        using pointer_to_member_type = glad_ctx_ptr_to_mem_PtrToMem_ptr_type<R, Args...>;

        gl_function(pointer_to_member_type ptrToMem)
            : m_PtrToMem{ptrToMem}
        {}

        gl_function(debugging_mode_off_type, pointer_to_member_type ptrToMem)
            : gl_function{ptrToMem}
        {
            static_assert(Mode == debugging_mode::none);
        }

        gl_function(nullptr_t) = delete;

        gl_function(debugging_mode_off_type, nullptr_t) = delete;

        [[nodiscard]]
        R operator()(const extended_context& ctx, Args... args, std::source_location loc = std::source_location::current()) const {
            const auto name{get_name(ctx.glad_context())};
            ctx.invoke_prologue(Mode, name, loc);
            cached_result v{get_validated_fn_ptr(ctx, loc), args...};
            ctx.invoke_epilogue(Mode, name, loc);

            return v.get();
        }
    private:
        pointer_to_member_type m_PtrToMem;

        [[nodiscard]]
        function_pointer_type<R, Args...> get_validated_fn_ptr(const extended_context& ctx, std::source_location loc) const {
            auto f{ctx.glad_context().*m_PtrToMem};
            return f ? f : throw std::runtime_error{std::format("gl_function: attempting to construct with a nullptr coming via {}", to_string(loc))};
        }

        [[nodiscard]]
        std::string_view get_name(const GladGLContext& ctx) const {
            const auto offset{std::bit_cast<uintptr_t>(&(ctx.*m_PtrToMem)) - std::bit_cast<uintptr_t>(&ctx)};
            const auto index{(offset - glad_ctx_member_info[0].offset) / sizeof(void*)};
            static_assert((glad_ctx_member_info.back().offset - glad_ctx_member_info.front().offset) / sizeof(int*) == (glad_ctx_member_info.size() - 1));
            assert(index < glad_ctx_member_info.size());

            return glad_ctx_member_info[index].name;
        }

        struct void_result {};

        class cached_result {
            std::conditional_t<std::is_void_v<R>, void_result, R> m_Value;
        public:
            template<class Fn>
                requires std::is_invocable_r_v<R, Fn, Args...> && (!std::is_void_v<R>)
            cached_result(Fn f, Args... args)
                : m_Value{f(args...)}
            {
            }

            template<class Fn>
                requires std::is_invocable_r_v<R, Fn, Args...> && std::is_void_v<R>
            cached_result(Fn f, Args... args)
            {
                f(args...);
            }

            [[nodiscard]]
            R get() const noexcept { return m_Value; }

            void get() const noexcept requires std::is_void_v<R> {}
        };
    };

    template<class R, class... Args>
    gl_function(glad_ctx_ptr_to_mem_PtrToMem_ptr_type<R, Args...>) -> gl_function<R(Args...)>;

    template<class R, class...Args>
    gl_function(debugging_mode_off_type, glad_ctx_ptr_to_mem_PtrToMem_ptr_type<R, Args...>) -> gl_function<R(Args...), debugging_mode::none>;
}