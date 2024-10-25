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

    void check_for_basic_errors(std::source_location loc)
    {
        if(!glGetError)
            throw std::runtime_error{"Null OpenGL function pointer"};

        std::string errorMessage{};
        error_codes errorCode{};
        while((errorCode = error_codes{glGetError()}) != error_codes::none)
        {
            errorMessage += to_string(errorCode);
            errorMessage += '\n';
        }

        if(!errorMessage.empty())
            throw std::runtime_error{std::format("OpenGL error detected in file {}, line {}:\n{}", fs::path{loc.file_name()}.generic_string(), loc.line(), errorMessage)};
    }
}