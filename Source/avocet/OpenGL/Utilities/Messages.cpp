////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Utilities/Messages.hpp"

#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

#include <format>

#include "glad/gl.h"

namespace avocet::opengl {
    std::string& trim(std::string& paddedMessage, GLsizei len) {
        const auto length{checked_conversion_to<std::size_t>(len)};

        if(length > paddedMessage.size())
            throw std::logic_error{std::format("Length of message {} cannot exceed padded length {}", len, paddedMessage.size())};

        const std::size_t trimLen{((length > 0) && paddedMessage[length - 1] == '\0') ? length - 1 : length};
        paddedMessage.resize(trimLen);

        return paddedMessage;
    }
}