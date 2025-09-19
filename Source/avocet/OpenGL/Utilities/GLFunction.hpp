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
    template<class> class gl_function;

    template<class R, class... Args>
    using function_pointer_type = R(*)(Args...);

    template<class R, class... Args>
    using glad_ctx_ptr_to_mem_ptr_to_mem_ptr_type = function_pointer_type<R, Args...> GladGLContext::*;

    template<class R, class... Args>
    class [[nodiscard]] gl_function<R(Args...)> {
    public:
        using pointer_to_member_type = glad_ctx_ptr_to_mem_ptr_to_mem_ptr_type<R, Args...>;

        gl_function(pointer_to_member_type ptrToMem)
            : m_PtrToMem{ptrToMem}
        {}

        gl_function(nullptr_t) = delete;

        [[nodiscard]]
        R operator()(const decorated_context& ctx, Args... args, std::source_location loc = std::source_location::current()) const {
            return invoke(ctx, ctx.debug_mode(), args..., loc);
        }

        [[nodiscard]]
        R operator()(const decorated_context& ctx, debugging_mode_off_type, Args... args, std::source_location loc = std::source_location::current()) const {
            return invoke(ctx, debugging_mode::off, args..., loc);
        }
    private:
        pointer_to_member_type m_PtrToMem;

        [[nodiscard]]
        R invoke(const decorated_context& ctx, debugging_mode mode, Args... args, std::source_location loc = std::source_location::current()) const {
            return ctx.invoke_decorated({.debug_mode{mode}, .name{get_name(ctx)}, .loc{loc}}, get_validated_fn_ptr(ctx, loc), args...);
        }

        [[nodiscard]]
        function_pointer_type<R, Args...> get_validated_fn_ptr(const decorated_context& ctx, std::source_location loc) const {
            auto f{ctx.glad_context().*m_PtrToMem};
            return f ? f : throw std::runtime_error{std::format("gl_function: attempting to invoke a null function pointer coming via {}", to_string(loc))};
        }

        [[nodiscard]]
        std::string_view get_name(const decorated_context& ctx) const {
            constexpr auto totalStride{
                glad_ctx_member_info.back().offset - glad_ctx_member_info.front().offset
            };

            static_assert(totalStride / sizeof(void*) == (glad_ctx_member_info.size() - 1));

            const auto offset{
                  std::bit_cast<std::uintptr_t>(&(ctx.glad_context().*m_PtrToMem))
                - std::bit_cast<std::uintptr_t>(&(ctx.glad_context()))
            };

            const auto index{(offset - glad_ctx_member_info.front().offset) / sizeof(void*)};
            if(index >= glad_ctx_member_info.size())
                throw std::runtime_error{std::format("gl_function::get_name index {} out of bounds", index)};

            return glad_ctx_member_info[index].name;
        }
    };

    template<class R, class... Args>
    gl_function(glad_ctx_ptr_to_mem_ptr_to_mem_ptr_type<R, Args...>) -> gl_function<R(Args...)>;
}