////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "avocet/Core/Formatting/Formatting.hpp"

#include <filesystem>

namespace avocet {
    namespace fs = std::filesystem;

    [[nodiscard]]
    std::string to_string(std::source_location loc) { return std::format("{}, line {}", fs::path{loc.file_name()}.generic_string(), loc.line()); }
}