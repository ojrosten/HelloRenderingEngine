////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

#include <filesystem>

namespace avocet::testing {
    [[nodiscard]]
    std::filesystem::path get_vertex_shader_dir();

    [[nodiscard]]
    std::filesystem::path get_fragment_shader_dir();

    [[nodiscard]]
    std::filesystem::path get_image_dir();
}