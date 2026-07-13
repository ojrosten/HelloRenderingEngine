////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Utilities/ArithmeticCasts.hpp"

#include <string>

#include "glad/gl.h"

namespace avocet::opengl {
    inline std::string& trim(std::string& paddedMessage, GLsizei len) {
        const auto length{checked_conversion_to<std::size_t>(len)};
        const std::size_t trimLen{((length > 0) && paddedMessage[length - 1] == '\0') ? length - 1 : length};
        paddedMessage.resize(trimLen);

        return paddedMessage;
    }
}