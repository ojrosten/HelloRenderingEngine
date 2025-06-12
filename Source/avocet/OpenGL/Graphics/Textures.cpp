////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#include "avocet/OpenGL/Graphics/Textures.hpp"

namespace avocet::opengl {
    namespace {
        [[nodiscard]]
        constexpr texture_internal_format to_internal_format(texture_format format, colour_space_flavour colourSpace) {
            const bool isLinear{colourSpace == colour_space_flavour::linear};

            switch(format) {
            case texture_format::red  : return            texture_internal_format::red;
            case texture_format::rg   : return            texture_internal_format::rg;
            case texture_format::rgb  : return isLinear ? texture_internal_format::rgb  : texture_internal_format::srgb;
            case texture_format::rgba : return isLinear ? texture_internal_format::rgba : texture_internal_format::srgba;
            }

            throw std::runtime_error{std::format("to_internal_format: unrecognized value of texture_format, {}", to_gl_enum(format))};
        }

        [[nodiscard]]
        constexpr texture_format to_format(colour_channels numChannels)
        {
            switch(numChannels.raw_value())
            {
            case 1: return texture_format::red;
            case 2: return texture_format::rg;
            case 3: return texture_format::rgb;
            case 4: return texture_format::rgba;
            }

            throw std::runtime_error{std::format("{} channels requested, but it must be in the range [1,4]", numChannels)};
        }
    }
}