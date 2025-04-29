////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/Graphics/Core/Image.hpp"
#include <format>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace avocet {
    namespace fs = std::filesystem;

    [[nodiscard]]
    image image::make(const fs::path& texture, vertically_flipped flip) {
        if(!fs::exists(texture))
            throw std::runtime_error{std::format("image {} not found", texture.generic_string())};

        //std::lock_guard lock{st_Mutex};

        stbi_set_flip_vertically_on_load(static_cast<bool>(flip));
        int width{}, height{}, numChannels{};

        if(auto pData{stbi_load(texture.generic_string().c_str(), &width, &height, &numChannels, 0)}; pData != nullptr)
            return {width, height, numChannels, pData};

        throw std::runtime_error{std::format("Failed to load image {}", texture.generic_string())};
    }

    void image::file_deleter::operator()(data_type* ptr) const {
        stbi_image_free(ptr);
    }
}
