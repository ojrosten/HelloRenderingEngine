////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "FileSystem.hpp"

#include <format>
#include <source_location>

namespace {
    namespace fs = std::filesystem;

    [[nodiscard]]
    fs::path get_dir(std::string_view dirName) {
        if(const fs::path file{std::source_location::current().file_name()}; file.is_absolute()) {
            if(const auto dir{file.parent_path().parent_path() / dirName}; fs::exists(dir)) {
                return dir;
            }
            else
                throw std::runtime_error{std::format("Unable to find {} directory {}", dirName, dir.generic_string())};
        }

        throw std::runtime_error{"Relative paths not supported"};
    }


    [[nodiscard]]
    fs::path get_shader_dir() { return get_dir("Shaders"); }
}

namespace avocet::testing {

    [[nodiscard]]
    fs::path get_fragment_shader_dir() { return get_shader_dir() / "Fragment"; }

    [[nodiscard]]
    fs::path get_vertex_shader_dir() { return get_shader_dir() / "Vertex"; }

    [[nodiscard]]
    fs::path get_image_dir() { return get_dir("Images"); }
}