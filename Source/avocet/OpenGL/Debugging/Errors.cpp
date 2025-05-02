////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Debugging/Errors.hpp"
#include "avocet/OpenGL/Graphics/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <vector>

#if defined(_MSC_VER)
    #include <experimental/generator>
    #define STD_GENERATOR std::experimental::generator
#elif defined(__linux__)
    #include <generator>
    #define STD_GENERATOR std::generator
#endif

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
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

        [[nodiscard]]
        std::string to_string(error_code e) {
            using enum error_code;
            switch(e) {
            case none:
                return "None";
            case invalid_enum:
                return "Invalid enum";
            case invalid_value:
                return "Invalid value";
            case invalid_operation:
                return "Invalid operation";
            case invalid_framebuffer_operation:
                return "Invalid framebuffer operation";
            case stack_overflow:
                return "Stack overflow";
            case stack_underflow:
                return "Stack underflow";
            case out_of_memory:
                return "Out of memory";
            }

            throw std::runtime_error{"error_code: unrecognized option"};
        }

        enum class debug_source : GLenum {
            api             = GL_DEBUG_SOURCE_API,
            window_system   = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
            shader_compiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
            third_party     = GL_DEBUG_SOURCE_THIRD_PARTY,
            application     = GL_DEBUG_SOURCE_APPLICATION,
            other           = GL_DEBUG_SOURCE_OTHER
        };

        [[nodiscard]]
        std::string to_string(debug_source e) {
            using enum debug_source;

            switch(e) {
            case api:             return "API";
            case window_system:   return "Window System";
            case shader_compiler: return "Shader Compiler";
            case third_party:     return "Third Party";
            case application:     return "Application";
            case other:           return "Other";
            }

            throw std::runtime_error{"debug_source: unrecognized option"};
        }

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

        [[nodiscard]]
        std::string to_string(debug_type e) {
            using enum debug_type;

            switch(e) {
            case error:                return "Error";
            case deprecated_behaviour: return "Deprecated Behaviour";
            case undefined_behaviour:  return "Undefined Behaviour";
            case portability:          return "Portability";
            case performance:          return "Performance";
            case marker:               return "Marker";
            case push_group:           return "Push Group";
            case pop_group:            return "Pop Group";
            case other:                return "Other";
            }

            throw std::runtime_error{"debug_type: unrecognized option"};
        }

        enum class debug_severity : GLenum {
            high         = GL_DEBUG_SEVERITY_HIGH,
            medium       = GL_DEBUG_SEVERITY_MEDIUM,
            low          = GL_DEBUG_SEVERITY_LOW,
            notification = GL_DEBUG_SEVERITY_NOTIFICATION
        };

        [[nodiscard]]
        std::string to_string(debug_severity e) {
            using enum debug_severity;

            switch(e) {
            case high:         return "High";
            case medium:       return "Medium";
            case low:          return "Low";
            case notification: return "Notification";
            }

            throw std::runtime_error{"debug_severity: unrecognized option"};
        }

        [[nodiscard]]
        GLint get_max_message_length(std::source_location loc) {
            GLint maxLen{};
            gl_function{unchecked_debug_output, glGetIntegerv, loc}(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxLen);
            return maxLen;
        }

        struct debug_info {
            debug_severity severity{};
            std::string message{};
        };

        [[nodiscard]]
        std::optional<debug_info> get_next_message(std::source_location loc) {
            const static GLint maxLen{get_max_message_length(loc)};

            std::string message(maxLen, ' ');
            GLenum source{}, type{}, severity{};
            GLuint id{};
            GLsizei length{};

            const auto numFound{gl_function{unchecked_debug_output, glGetDebugMessageLog}(1, to_gl_sizei(message.size()), &source, &type, &id, &severity, &length, message.data())};
            const auto trimLen{((length > 0) && message[length - 1] == '\0') ? length - 1 : length};
            message.resize(trimLen);

            if(numFound)
                return {{debug_severity{severity},
                            std::format("Source: {}; Type: {}; Severity: {}\n{}",
                                        to_string(debug_source{source}),
                                        to_string(debug_type{type}),
                                        to_string(debug_severity{severity}),
                                        message)}};
            
            return std::nullopt;
        }

        std::string compose_error_message(std::string_view errorMessage, std::source_location loc) {
            return std::format("OpenGL error detected in {}:\n{}\n", avocet::opengl::to_string(loc), errorMessage);
        }
#ifndef __clang__
        [[nodiscard]]
        STD_GENERATOR<error_code> get_errors(num_messages maxNum) {
            for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
                const error_code e{gl_function{unchecked_debug_output, glGetError}()};
                if(e == error_code::none) co_return;

                co_yield e;
            }
        }

        [[nodiscard]]
        STD_GENERATOR<debug_info> get_messages(num_messages maxNum, std::source_location loc) {
            for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
                const std::optional<debug_info> message{get_next_message(loc)};
                if(!message) co_return;

                co_yield message.value();
            }
        }
#else
        [[nodiscard]]
        std::vector<error_code> get_errors(num_messages maxNum) {
            std::vector<error_code> errors;
            for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
                const error_code e{gl_function{unchecked_debug_output, glGetError}()};
                if(e == error_code::none) break;

                errors.push_back(e);
            }

            return errors;
        }

        [[nodiscard]]
        std::vector<debug_info> get_messages(num_messages maxNum, std::source_location loc) {
            std::vector<debug_info> info;
            for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
                const std::optional<debug_info> message{get_next_message(loc)};
                if(!message) break;

                info.push_back(message.value());
            }

            return info;
        }
#endif
    }

    [[nodiscard]]
    std::string to_string(std::source_location loc) { return std::format("{}, line {}", fs::path{loc.file_name()}.generic_string(), loc.line()); }

    void check_for_basic_errors(num_messages maxNum, std::source_location loc)
    {
        const std::string errorMessage{
            std::ranges::fold_left(
                get_errors(maxNum),
                std::string{},
                [](std::string message, error_code e){
                    const auto separator{message.empty() ? "" : "\n"};
                    return (message += separator) += to_string(e);
                }
            )
        };

        if(!errorMessage.empty())
            throw std::runtime_error{compose_error_message(errorMessage, loc)};
    }

    void check_for_advanced_errors(num_messages maxNum, std::source_location loc) {
        const std::string errorMessage{
            std::ranges::fold_left(
                get_messages(maxNum, loc),
                std::string{},
                [](std::string message, const debug_info& info){
                    const auto separator{message.empty() ? "" : "\n\n"};
                    if(info.severity != debug_severity::notification) {
                        (message += separator) += info.message;
                    }

                    return message;
                }
            )
        };

        if(!errorMessage.empty())
            throw std::runtime_error{compose_error_message(errorMessage, loc)};
    }
}