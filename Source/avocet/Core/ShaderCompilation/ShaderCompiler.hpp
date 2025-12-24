////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include "shaderc/shaderc.hpp"

#include <filesystem>
#include <vector>

namespace avocet {
    [[nodiscard]]
    std::vector<uint32_t> compile_glsl_to_spv(const std::filesystem::path& sourceFile, shaderc_shader_kind kind, std::string_view label);
}