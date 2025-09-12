////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/OpenGL/Debugging/DebugMode.hpp"
#include "avocet/OpenGL/Utilities/Version.hpp"

#include <algorithm>
#include <source_location>
#include <string>

#if defined(_MSC_VER)
#include <experimental/generator>
#define STD_GENERATOR std::experimental::generator
#elif defined (__clang__)
#elif defined(__GNUG__)
#include <generator>
#define STD_GENERATOR std::generator
#endif

namespace avocet::opengl {
    enum class error_code : GLenum {
        none                          = GL_NO_ERROR,
        invalid_enum                  = GL_INVALID_ENUM,
        invalid_value                 = GL_INVALID_VALUE,
        invalid_operation             = GL_INVALID_OPERATION,
        invalid_framebuffer_operation = GL_INVALID_FRAMEBUFFER_OPERATION,
        stack_overflow                = GL_STACK_OVERFLOW,
        stack_underflow               = GL_STACK_UNDERFLOW,
        out_of_memory                 = GL_OUT_OF_MEMORY,
    };

    enum class debug_severity : GLenum {
        high = GL_DEBUG_SEVERITY_HIGH,
        medium = GL_DEBUG_SEVERITY_MEDIUM,
        low = GL_DEBUG_SEVERITY_LOW,
        notification = GL_DEBUG_SEVERITY_NOTIFICATION
    };

    [[nodiscard]]
    std::string to_string(debug_severity e);

    struct debug_info {
        debug_severity severity{};
        std::string message{};
    };

    [[nodiscard]]
    std::string to_string(error_code e);

    [[nodiscard]]
    std::string to_string(std::source_location loc);

    struct num_messages { std::size_t value{}; };

    struct default_error_code_processor {
        [[nodiscard]]
        std::string operator()(std::string message, error_code e) const {
            const auto separator{message.empty() ? "" : "\n"};
            return (message += separator) += to_string(e);
        }
    };

    struct default_debug_info_processor {
        [[nodiscard]]
        std::string operator()(std::string message, const debug_info& info) const {
            const auto separator{message.empty() ? "" : "\n\n"};
            if(info.severity != debug_severity::notification) {
                (message += separator) += info.message;
            }

            return message;
        }
    };

#ifdef __cpp_lib_generator
    [[nodiscard]]
    STD_GENERATOR<error_code> get_errors(const extended_context& ctx, num_messages maxNum);

    [[nodiscard]]
    STD_GENERATOR<debug_info> get_messages(const extended_context& ctx, num_messages maxNum);
#else
    [[nodiscard]]
    std::vector<error_code> get_errors(const extended_context& ctx, num_messages maxNum);

    [[nodiscard]]
    std::vector<debug_info> get_messages(const extended_context& ctx, num_messages maxNum);
#endif

    std::string compose_error_message(std::string_view errorMessage, std::string_view fnName, std::source_location loc);

    template<class Fn = default_error_code_processor>
        requires std::is_invocable_r_v<std::string, Fn, std::string, error_code>
    void check_for_basic_errors(const extended_context& ctx, num_messages maxNum, std::string_view fnName, std::source_location loc, Fn errorProcessor = {}) {
        const std::string errorMessage{std::ranges::fold_left(get_errors(ctx, maxNum), std::string{}, errorProcessor)};

        if(!errorMessage.empty())
            throw std::runtime_error{compose_error_message(errorMessage, fnName, loc)};
    }

    template<class Fn = default_debug_info_processor>
        requires std::is_invocable_r_v<std::string, Fn, std::string, debug_info>
    void check_for_advanced_errors(const extended_context& ctx, num_messages maxNum, std::string_view fnName, std::source_location loc, Fn errorProcessor = {}) {
        const std::string errorMessage{std::ranges::fold_left(get_messages(ctx, maxNum), std::string{}, errorProcessor)};

        if(!errorMessage.empty())
            throw std::runtime_error{compose_error_message(errorMessage, fnName, loc)};
    }

    [[nodiscard]]
    constexpr bool debug_output_supported(opengl_version version) noexcept {
        return (version.major > 3) && (version.minor >= 3);
    }

    [[nodiscard]]
    inline bool debug_output_supported(const extended_context& ctx) {
        return debug_output_supported(get_opengl_version(ctx));
    }

    [[nodiscard]]
    inline bool object_labels_activated(const extended_context& ctx) { return debug_output_supported(ctx); }
}
