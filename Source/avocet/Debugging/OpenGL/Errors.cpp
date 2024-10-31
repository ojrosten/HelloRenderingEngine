////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Debugging/OpenGL/Errors.hpp"
#include "avocet/Graphics/OpenGL/GLFunction.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <stdexcept>

#include "glad/gl.h"

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
        enum class error_codes : GLuint {
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
        std::string to_string(error_codes e) {
            using enum error_codes;
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

            throw std::runtime_error{"error_codes: unrecognized option"};
        }

        enum class debug_sources : GLenum {
            api             = GL_DEBUG_SOURCE_API,
            window_system   = GL_DEBUG_SOURCE_WINDOW_SYSTEM,
            shader_compiler = GL_DEBUG_SOURCE_SHADER_COMPILER,
            third_party     = GL_DEBUG_SOURCE_THIRD_PARTY,
            application     = GL_DEBUG_SOURCE_APPLICATION,
            other           = GL_DEBUG_SOURCE_OTHER
        };

        [[nodiscard]]
        std::string to_string(debug_sources e) {
            using enum debug_sources;

            switch(e) {
            case api:             return "API";
            case window_system:   return "Window System";
            case shader_compiler: return "Shader Compiler";
            case third_party:     return "Third Party";
            case application:     return "Application";
            case other:           return "Other";
            }

            throw std::runtime_error{"debug_sources: unrecognized option"};
        }

        enum class debug_types : GLenum {
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
        std::string to_string(debug_types e) {
            using enum debug_types;

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

            throw std::runtime_error{"debug_types: unrecognized option"};
        }

        enum class debug_severities : GLenum {
            high         = GL_DEBUG_SEVERITY_HIGH,
            medium       = GL_DEBUG_SEVERITY_MEDIUM,
            low          = GL_DEBUG_SEVERITY_LOW,
            notification = GL_DEBUG_SEVERITY_NOTIFICATION
        };

        [[nodiscard]]
        std::string to_string(debug_severities e) {
            using enum debug_severities;

            switch(e) {
            case high:         return "High";
            case medium:       return "Medium";
            case low:          return "Low";
            case notification: return "Notification";
            }

            throw std::runtime_error{"debug_severities: unrecognized option"};
        }

        [[nodiscard]]
        GLint get_max_message_length(std::source_location loc) {
            GLint maxLen{};
            gl_function{debug_output_unchecked, glGetIntegerv, loc}(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxLen);
            return maxLen;
        }

        struct debug_info {
            debug_severities severity{};
            std::string message{};
        };

        [[nodiscard]]
        std::optional<debug_info> get_next_message(std::source_location loc) {
            const static GLint maxLen{get_max_message_length(loc)};

            std::string message(maxLen, ' ');
            GLenum source{}, type{}, severity{};
            GLuint id{};
            GLsizei length{};

            const auto numFound{gl_function{debug_output_unchecked, glGetDebugMessageLog, loc}(1, static_cast<GLsizei>(message.size()), &source, &type, &id, &severity, &length, message.data())};
            const auto trimLen{((length > 0) && message[length - 1] == '\0') ? length - 1 : length};
            message.resize(trimLen);

            if(numFound)
                return {{debug_severities{severity},
                            std::format("Source: {}; Type: {}; Severity: {}\n{}",
                                        to_string(debug_sources{source}),
                                        to_string(debug_types{type}),
                                        to_string(debug_severities{severity}),
                                        message)}};
            
            return std::nullopt;
        }

        std::string compose_error_message(std::string_view errorMessage, std::source_location loc) {
            return std::format("OpenGL error detected in file {}, line {}:\n{}", fs::path{loc.file_name()}.generic_string(), loc.line(), errorMessage);
        }
    }

    [[nodiscard]]
    std::string to_string(std::source_location loc) { return std::format("{}, line {}", fs::path{loc.file_name()}.generic_string(), loc.line()); }

    void check_for_basic_errors(std::source_location loc)
    {
        std::string errorMessage{};
        error_codes errorCode{};
        while((errorCode = error_codes{gl_function{debug_output_unchecked, glGetError, loc}()}) != error_codes::none)
        {
            errorMessage += to_string(errorCode);
            errorMessage += '\n';
        }

        if(!errorMessage.empty())
            throw std::runtime_error{std::format("OpenGL error detected in {}:\n{}", to_string(loc), errorMessage)};
    }

    void check_for_advanced_errors(std::source_location loc) {
        std::string errorMessage{};
        std::optional<debug_info> currentInfo{};
        while((currentInfo = get_next_message(loc)) != std::nullopt) {
            const auto& info{currentInfo.value()};
            if(info.severity == debug_severities::notification)
                std::cerr << info.message << '\n';
            else {
                errorMessage += info.message;
                errorMessage += '\n';
            }
        }

        if(!errorMessage.empty())
            throw std::runtime_error{compose_error_message(errorMessage, loc)};
    }
}