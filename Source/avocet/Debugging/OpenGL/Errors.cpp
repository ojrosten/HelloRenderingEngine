////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Debugging/OpenGL/Errors.hpp"
#include "avocet/Graphics/OpenGL/GLFunction.hpp"
#include "avocet/Utilities/OpenGL/Casts.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <iostream>
#include <ranges>
#include <stdexcept>

#if defined(_MSC_VER)
    #include <experimental/generator>
    #define STD_GENERATOR std::experimental::generator
#elif defined(__linux__)
    #include <generator>
    #define STD_GENERATOR std::generator
#endif

#include "glad/gl.h"

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
            return std::format("OpenGL error detected in {}:\n{}", avocet::opengl::to_string(loc), errorMessage);
        }

        struct max_num_errors { std::size_t value{}; };

        [[nodiscard]]
        STD_GENERATOR<error_code> get_errors(max_num_errors bound) {
            for([[maybe_unused]] auto _ : std::views::iota(0u, bound.value)) {
                const error_code e{gl_function{unchecked_debug_output, glGetError}()};
                if(e == error_code::none) co_return;

                co_yield e;
            }
        }

        [[nodiscard]]
        auto get_errors2(max_num_errors bound) {
           return  std::views::iota(0u, bound.value)
                 | std::views::transform([](auto){ 
                       error_code e{gl_function{unchecked_debug_output, glGetError}()}; 
                        return e;
                   })
                 | std::views::take_while([](error_code e) { 
                       return e != error_code::none;
                   });
        }


        [[nodiscard]]
        STD_GENERATOR<debug_info> get_messages(max_num_errors bound, std::source_location loc) {
            for([[maybe_unused]] auto _ : std::views::iota(0u, bound.value)) {
                const auto optMessage{get_next_message(loc)};
                if(!optMessage) co_return;

                co_yield optMessage.value();
            }
        }

        struct gl_error{
            using difference_type = std::make_signed_t<GLenum>;

            static error_code get_error() { return error_code{gl_function{unchecked_debug_output, glGetError}()}; }

            error_code error{get_error()};
            std::size_t count{};

            gl_error& operator++() {
                ++count;
                error = get_error();
                return *this;
            }

            gl_error operator++(int);

            [[nodiscard]]
            operator error_code() const noexcept { return error; }
        };

        class error_bound {
            max_num_errors m_Max{10};
        public:
            error_bound() = default;

            explicit error_bound(max_num_errors maxNum)
                : m_Max{maxNum}
            {}

            [[nodiscard]]
            bool operator==(const gl_error& rhs) const noexcept { return (rhs.error == error_code::none) || (rhs.count == m_Max.value); }
        };

    }

    [[nodiscard]]
    std::string to_string(std::source_location loc) { return std::format("{}, line {}", fs::path{loc.file_name()}.generic_string(), loc.line()); }

    void check_for_basic_errors(std::source_location loc)
    {
        std::string errorMessage{
            std::ranges::fold_left(
                get_errors2(max_num_errors{10}),
                std::string{},
                [](std::string message, const error_code& e){
                    return message += to_string(e) += "\n\n";
                }
            )
        };

        if(!errorMessage.empty()) {
            errorMessage.pop_back();
            throw std::runtime_error{compose_error_message(errorMessage, loc)};
        }
    }

    void check_for_advanced_errors(std::source_location loc) {
        std::string errorMessage{
            std::ranges::fold_left(
                get_messages(max_num_errors{10}, loc),
                std::string{},
                [](std::string message, debug_info info){
                    if(info.severity == debug_severity::notification) {
                        std::cerr << info.message << '\n';
                        return message;
                    }

                    return message += info.message += "\n\n";
                }
            )
        };

        if(!errorMessage.empty()) {
            errorMessage.pop_back();
            throw std::runtime_error{compose_error_message(errorMessage, loc)};
        }
    }
}