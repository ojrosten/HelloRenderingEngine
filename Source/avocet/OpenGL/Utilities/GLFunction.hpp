////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/Errors.hpp"

#include <algorithm>
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

    template<class T>
    class value_from_invocation {
        T m_Value;
    public:
        template<class Fn>
            requires std::is_invocable_r_v<T, Fn>
        value_from_invocation(Fn f)
            : m_Value{f()}
        {
        }

        T get() const noexcept { return m_Value; }
    };

    template<>
    class value_from_invocation<void> {
    public:
        template<class Fn>
            requires std::is_invocable_r_v<void, Fn>
        value_from_invocation(Fn f)
        {
            f();
        }

        void get() const noexcept {}
    };

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
            std::exception_ptr eptr{};
            value_from_invocation<R> v{[&, this]() { return invoke(ctx, args..., eptr, loc); }};
            if(eptr)
                std::rethrow_exception(eptr);

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
            const auto offset{reinterpret_cast<uintptr_t>(&(ctx.*m_PtrToMem)) - reinterpret_cast<uintptr_t>(&ctx)};
            const auto index{(offset - glad_ctx_member_info[0].offset) / sizeof(int*)};
            static_assert((glad_ctx_member_info.back().offset - glad_ctx_member_info.front().offset) / sizeof(int*) == (glad_ctx_member_info.size() - 1));
            assert(index < glad_ctx_member_info.size());

            return glad_ctx_member_info[index].name;
        }

        [[nodiscard]]
        R invoke(const extended_context& ctx, Args... args, std::exception_ptr& eptr, std::source_location loc = std::source_location::current()) const {
            decoration_invoker invoker{ctx, get_name(ctx.glad_context()), loc, eptr};
            return get_validated_fn_ptr(ctx, loc)(args...);
        }

        class [[nodiscard]] decoration_invoker {
            const extended_context& m_Context;
            std::string_view m_Name;
            std::source_location m_Loc;
            std::exception_ptr& m_ExceptionPtr{};
        public:
            explicit decoration_invoker(const extended_context& ctx, std::string_view name, std::source_location loc, std::exception_ptr& ptr)
                : m_Context{ctx}
                , m_Name{name}
                , m_Loc{loc}
                , m_ExceptionPtr{ptr}
            {
                m_Context.invoke_prologue(Mode, m_Name, m_Loc);
            }

            ~decoration_invoker() {
                try {
                    m_Context.invoke_epilogue(Mode, m_Name, m_Loc);
                }
                catch(...) {
                    m_ExceptionPtr = std::current_exception();
                }
            }
        };
    };

    template<class R, class... Args>
    gl_function(glad_ctx_ptr_to_mem_PtrToMem_ptr_type<R, Args...>) -> gl_function<R(Args...)>;

    template<class R, class...Args>
    gl_function(debugging_mode_off_type, glad_ctx_ptr_to_mem_PtrToMem_ptr_type<R, Args...>) -> gl_function<R(Args...), debugging_mode::none>;
}