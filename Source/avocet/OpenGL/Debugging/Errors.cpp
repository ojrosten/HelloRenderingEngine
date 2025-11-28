////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Debugging/Errors.hpp"
#include "avocet/OpenGL/Context/GLFunction.hpp"
#include "avocet/OpenGL/Utilities/Casts.hpp"

#include <filesystem>
#include <format>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
        [[nodiscard]]
        GLint get_max_message_length(const decorated_context& ctx) {
            GLint maxLen{};
            gl_function{&GladGLContext::GetIntegerv}(ctx, debugging_mode_off, GL_MAX_DEBUG_MESSAGE_LENGTH, &maxLen);
            return maxLen;
        }

        [[nodiscard]]
        std::optional<debug_info> get_next_message(const decorated_context& ctx) {
            const static GLint maxLen{get_max_message_length(ctx)};

            std::string message(maxLen, ' ');
            GLenum source{}, type{}, severity{};
            GLuint id{};
            GLsizei length{};

            const auto numFound{gl_function{&GladGLContext::GetDebugMessageLog}(ctx, debugging_mode_off, 1, to_gl_sizei(message.size()), &source, &type, &id, &severity, &length, message.data())};
            const auto trimLen{((length > 0) && message[length - 1] == '\0') ? length - 1 : length};
            message.resize(trimLen);

            if(numFound)
                return {{id, debug_source{source}, debug_type{type}, debug_severity{severity}, message}};
            
            return std::nullopt;
        }

        /// The code below exemplifies how to use std::generator. However, since
        /// this is unavailable on libc++, an alternative approach must be taken.
        /// This caused some test output to differe between platforms, due to the
        /// defaulted final argument of gl_function picking up the source_location
        /// from different locations. Pulling the gl_function out into a helper
        /// function gives platform-independent output. Once std::generator is
        /// available everywhere, the platform-dependent code can be removed.
        [[nodiscard]]
        error_code get_error(const decorated_context& ctx) { return error_code{gl_function{&GladGLContext::GetError}(ctx, debugging_mode_off)}; }

        /// lib++ does not currently support std::generator, a fact which we need to work around
        namespace libcpp_workaround {
#ifndef _MSC_VER
    #pragma GCC diagnostic ignored "-Wunused-function"
    #pragma GCC diagnostic push
#endif

            [[nodiscard]]
            std::vector<error_code> get_errors(const decorated_context& ctx, num_messages maxNum) {
                std::vector<error_code> errors;
                for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
                    const error_code e{get_error(ctx)};
                    if(e == error_code::none) break;

                    errors.push_back(e);
                }

                return errors;
            }

            [[nodiscard]]
            std::vector<debug_info> get_messages(const decorated_context& ctx, num_messages maxNum) {
                std::vector<debug_info> info;
                for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
                    const std::optional<debug_info> message{get_next_message(ctx)};
                    if(!message) break;

                    info.push_back(message.value());
                }

                return info;
            }

#ifndef _MSC_VER
    #pragma GCC diagnostic pop
#endif
        }
    }

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

    [[nodiscard]]
    std::string to_detailed_message(const debug_info& info) {
        return 
            std::format(
                "Id: {}; Source: {}; Type: {}; Severity: {}\n{}",
                info.id,
                to_string(debug_source{info.source}),
                to_string(debug_type{info.type}),
                to_string(debug_severity{info.severity}),
                info.message
            );
    }

    [[nodiscard]]
    std::string compose_error_message(std::string_view errorMessage, const error_message_info& info) {
        return std::format("OpenGL error detected following invocation of {} originating from {}:\n{}\n", info.fn_name, to_string(info.loc), errorMessage);
    }


#ifdef __cpp_lib_generator
    [[nodiscard]]
        STD_GENERATOR<error_code> get_errors(const decorated_context& ctx, num_messages maxNum) {
        for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
            const error_code e{get_error(ctx)};
            if(e == error_code::none) co_return;

            co_yield e;
        }
    }

    [[nodiscard]]
    STD_GENERATOR<debug_info> get_messages(const decorated_context& ctx, num_messages maxNum) {
        for([[maybe_unused]] auto _ : std::views::iota(0u, maxNum.value)) {
            const std::optional<debug_info> message{get_next_message(ctx)};
            if(!message) co_return;

            co_yield message.value();
        }
    }
#else
    [[nodiscard]]
    std::vector<error_code> get_errors(const decorated_context& ctx, num_messages maxNum) {
        return libcpp_workaround::get_errors(ctx, maxNum);
    }

    [[nodiscard]]
    std::vector<debug_info> get_messages(const decorated_context& ctx, num_messages maxNum) {
        return libcpp_workaround::get_messages(ctx, maxNum);
    }
#endif
}
