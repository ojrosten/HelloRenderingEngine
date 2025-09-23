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
#include <vector>

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

    enum class debug_source : GLenum {
        api             = GL_DEBUG_SOURCE_API,
        window_system   = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
        shader_compiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
        third_party     = GL_DEBUG_SOURCE_THIRD_PARTY,
        application     = GL_DEBUG_SOURCE_APPLICATION,
        other           = GL_DEBUG_SOURCE_OTHER
    };

    enum class debug_type : GLenum {
        error                = GL_DEBUG_TYPE_ERROR,
        deprecated_behaviour = GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,
        undefined_behaviour  = GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,
        portability          = GL_DEBUG_TYPE_PORTABILITY,
        performance          = GL_DEBUG_TYPE_PERFORMANCE,
        marker               = GL_DEBUG_TYPE_MARKER,
        push_group           = GL_DEBUG_TYPE_PUSH_GROUP,
        pop_group            = GL_DEBUG_TYPE_POP_GROUP,
        other                = GL_DEBUG_TYPE_OTHER
    };

    enum class debug_severity : GLenum {
        high         = GL_DEBUG_SEVERITY_HIGH,
        medium       = GL_DEBUG_SEVERITY_MEDIUM,
        low          = GL_DEBUG_SEVERITY_LOW,
        notification = GL_DEBUG_SEVERITY_NOTIFICATION
    };

    [[nodiscard]]
    std::string to_string(debug_source e);

    [[nodiscard]]
    std::string to_string(debug_severity e);

    [[nodiscard]]
    std::string to_string(debug_type e);

    struct debug_info {
        GLuint         id{};
        debug_source   source{};
        debug_type     type{};
        debug_severity severity{};
        std::string    message{};
    };

    [[nodiscard]]
    std::string to_detailed_message(const debug_info& info);

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
                (message += separator) += to_detailed_message(info);
            }

            return message;
        }
    };

#ifdef __cpp_lib_generator
    [[nodiscard]]
    STD_GENERATOR<error_code> get_errors(const decorated_context& ctx, num_messages maxNum);

    [[nodiscard]]
    STD_GENERATOR<debug_info> get_messages(const decorated_context& ctx, num_messages maxNum);
#else
    [[nodiscard]]
    std::vector<error_code> get_errors(const decorated_context& ctx, num_messages maxNum);

    [[nodiscard]]
    std::vector<debug_info> get_messages(const decorated_context& ctx, num_messages maxNum);
#endif

    struct error_message_info {
        std::string_view fn_name;
        std::source_location loc;
        num_messages max_reported;
    };

    [[nodiscard]]
    std::string compose_error_message(std::string_view errorMessage, const error_message_info& info);

    template<class Processor>
    inline constexpr bool is_error_code_processor_v{std::is_invocable_r_v<std::string, Processor, std::string, error_code>};

    template<class Processor>
    inline constexpr bool is_debug_info_processor_v{std::is_invocable_r_v<std::string, Processor, std::string, debug_info>};

    template<class ErrorProcessor>
        requires is_error_code_processor_v<ErrorProcessor>
    void check_for_basic_errors(const decorated_context& ctx, const error_message_info& info, ErrorProcessor processor)
    {
        const std::string errorMessage{std::ranges::fold_left(get_errors(ctx, info.max_reported), std::string{}, processor)};

        if(!errorMessage.empty())
            throw std::runtime_error{compose_error_message(errorMessage, info)};
    }

    template<class ErrorProcessor>
        requires is_debug_info_processor_v<ErrorProcessor>
    void check_for_advanced_errors(const decorated_context& ctx, const error_message_info& info, ErrorProcessor processor) {
        const std::string errorMessage{
            std::ranges::fold_left(get_messages(ctx, info.max_reported),  std::string{}, processor)
        };

        if(!errorMessage.empty())
            throw std::runtime_error{compose_error_message(errorMessage, info)};
    }

    [[nodiscard]]
    constexpr bool debug_output_supported(opengl_version version) noexcept {
        return (version.major > 3) && (version.minor >= 3);
    }

    [[nodiscard]]
    inline bool debug_output_supported(const decorated_context& ctx) {
        return debug_output_supported(get_opengl_version(ctx));
    }

    [[nodiscard]]
    inline bool object_labels_activated(const decorated_context& ctx) { return debug_output_supported(ctx); }
}
