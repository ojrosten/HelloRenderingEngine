////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Core/AssetManagement/Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace avocet {
    namespace fs = std::filesystem;

    void unique_image::file_unloader::operator()(value_type* ptr) const {
        stbi_image_free(ptr);
    }

    [[nodiscard]]
    unique_image unique_image::make(const std::filesystem::path& texturePath, flip_vertically flip) {
        if(!fs::exists(texturePath))
            throw std::runtime_error{std::format("unique_image: texture {} not found", texturePath.generic_string())};

        stbi_set_flip_vertically_on_load_thread(static_cast<bool>(flip));

        int width{}, height{}, channels{};
        auto pData{stbi_load(texturePath.generic_string().c_str(), &width, &height, &channels, 0)};
        if(!pData)
            throw std::runtime_error{std::format("unique_image: texture {} did not load", texturePath.generic_string())};

        return {pData, width, height, channels, alignment{1}};
    }
}
