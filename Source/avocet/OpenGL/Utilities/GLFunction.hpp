////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/Errors.hpp"

#include <algorithm>
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
            auto nameMaker{[this, &ctx]() { return get_name(ctx.glad_context()); }};
            ctx.invoke_prologue(Mode, nameMaker, loc);
            const auto ret{get_validated_fn_ptr(ctx, loc)(args...)};
            ctx.invoke_epilogue(Mode, nameMaker, loc);
            return ret;
        }

        void operator()(const extended_context& ctx, Args... args, std::source_location loc = std::source_location::current()) const
            requires std::is_void_v<R>
        {
            auto nameMaker{[this, &ctx]() { return get_name(ctx.glad_context()); }};
            ctx.invoke_prologue(Mode, nameMaker, loc);
            get_validated_fn_ptr(ctx, loc)(args...);
            ctx.invoke_epilogue(Mode, nameMaker, loc);
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
            if(index >= glad_ctx_member_info.size())
                throw std::runtime_error{std::format("offset index {} too big for the array of glad context member info", index)};

            /*auto found{std::ranges::lower_bound(glad_ctx_member_info, offset, std::ranges::less{}, [](const member_info& info) { return info.offset; })};
            if(found == glad_ctx_member_info.end())
                throw std::runtime_error{std::format("offset {} not found in GladGLContext", offset)};

            return found->name;*/

            return glad_ctx_member_info[index].name;
        }
    };

    template<class R, class... Args>
    gl_function(glad_ctx_ptr_to_mem_PtrToMem_ptr_type<R, Args...>) -> gl_function<R(Args...)>;

    template<class R, class...Args>
    gl_function(debugging_mode_off_type, glad_ctx_ptr_to_mem_PtrToMem_ptr_type<R, Args...>) -> gl_function<R(Args...), debugging_mode::none>;
}