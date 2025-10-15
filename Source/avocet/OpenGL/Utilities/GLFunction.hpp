////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/Errors.hpp"

#include <bit>
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
        R operator()(this const gl_function& self, const decorated_context& ctx, Args... args, std::source_location loc = std::source_location::current()) {
            return self.invoke(ctx, ctx.debug_mode(), args..., loc);
        }

        [[nodiscard]]
        R operator()(this const gl_function& self, const decorated_context& ctx, debugging_mode_off_type, Args... args, std::source_location loc = std::source_location::current()) {
            return self.invoke(ctx, debugging_mode::off, args..., loc);
        }
    private:
        pointer_to_member_type m_PtrToMem;

        struct named_fn_ptr {
            function_pointer_type<R, Args...> fn;
            std::string_view name;
        };

        [[nodiscard]]
        R invoke(this const gl_function& self, const decorated_context& ctx, debugging_mode mode, Args... args, std::source_location loc = std::source_location::current()) {
            const auto[fn, name]{self.get_validated_fn_ptr(ctx, loc)};
            return ctx.invoke_decorated({mode, name, loc}, fn, args...);
        }

        [[nodiscard]]
        std::string_view get_name(const decorated_context& ctx) const {
            static_assert(!glad_ctx_member_info.empty());

            const auto totalStride{
                glad_ctx_member_info.back().offset - glad_ctx_member_info.front().offset
            };
            static_assert(totalStride / sizeof(void*) == glad_ctx_member_info.size() - 1);

            const auto offset{
                  std::bit_cast<std::uintptr_t>(&(ctx.glad_context().*m_PtrToMem))
                - std::bit_cast<std::uintptr_t>(&ctx.glad_context())
            };

            const auto index{
                (offset - glad_ctx_member_info.front().offset) / sizeof(void*)
            };

            if(index >= glad_ctx_member_info.size())
                throw std::out_of_range{std::format("gl_function::get_name index {} out of bounds", index)};

            return glad_ctx_member_info[index].name;
        }

        [[nodiscard]]
        named_fn_ptr get_validated_fn_ptr(const decorated_context& ctx, std::source_location loc) const {
            named_fn_ptr namedFnPtr{ctx.glad_context().*m_PtrToMem, get_name(ctx)};
            return namedFnPtr.fn ? namedFnPtr : throw std::runtime_error{std::format("gl_function: attempting to invoke a null function pointer {} coming via {}", namedFnPtr.name, to_string(loc))};
        }
    };

    template<class R, class... Args>
    gl_function(glad_ctx_ptr_to_mem_ptr_to_mem_ptr_type<R, Args...>) -> gl_function<R(Args...)>;
}