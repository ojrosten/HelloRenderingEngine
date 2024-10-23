////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Graphics/OpenGL/Errors.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <ranges>
#include <stdexcept>

namespace avocet::opengl {
    namespace fs = std::filesystem;

    namespace {
        struct gl_error{
            using difference_type = GLint;

            static GLenum invoke_validated() { return glGetError ? glGetError() : throw std::runtime_error{"Null OpenGL function pointer"}; }

            GLenum value{invoke_validated()};

            gl_error& operator++() { value = invoke_validated(); return *this; }

            gl_error operator++(int);// { gl_error temp{};  value = invoke_validated(); return temp; }
        };

        struct no_error {
            [[nodiscard]]
            bool operator==(const gl_error& rhs) const noexcept { return rhs.value == GL_NO_ERROR; }
        };
    }

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

    void check_for_errors(std::source_location loc)
    {
        const auto errorMessage{
            std::ranges::fold_left(
                std::views::iota(gl_error{}, no_error{}),
                std::string{},
                [](std::string message, const gl_error& e){
                    return std::move(message) += (to_string(error_codes{e.value}) + "\n");
                })
        };

        /*error_codes errorCode;
        std::string errorMessage{};
        if(!glGetError)
            throw std::runtime_error{"Null OpenGL function pointer"};

        while((errorCode = error_codes{glGetError()}) != error_codes::none)
        {
            errorMessage += to_string(errorCode);
            errorMessage += '\n';
        }*/

        if(!errorMessage.empty())
            throw std::runtime_error{std::format("OpenGL error detected in file {}, line {}:\n{}", fs::path{loc.file_name()}.generic_string(), loc.line(), errorMessage)};
    }
}