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
        constexpr texture_internal_format to_internal_format(texture_format format, sampling_decoding colourSpace) {
            const bool noDecoding{colourSpace == sampling_decoding::none};

            switch(format) {
            case texture_format::red  : return              texture_internal_format::red;
            case texture_format::rg   : return              texture_internal_format::rg;
            case texture_format::rgb  : return noDecoding ? texture_internal_format::rgb  : texture_internal_format::srgb;
            case texture_format::rgba : return noDecoding ? texture_internal_format::rgba : texture_internal_format::srgba;
            }

            throw std::runtime_error{std::format("to_internal_format: unrecognized value of texture_format, {}", to_gl_enum(format))};
        }

        [[nodiscard]]
        constexpr int to_ogl_alignment(alignment rowAlignment) {
            if(rowAlignment.raw_value() > 8)
                throw std::runtime_error{std::format("Row alignment of {} bytes requested, but OpenGL only supports 1, 2, 4 and 8 bytes", rowAlignment)};

            return static_cast<int>(rowAlignment.raw_value());
        }
    }

    void texture_2d_lifecycle_events::configure(const resource_handle& h, const configurator& config) {
    }

    [[nodiscard]]
    unique_image texture_2d::do_extract_data(const texture_2d& tex2d, texture_format format, alignment rowAlignment) {
        using value_type = texture_2d::value_type;
        std::vector<value_type> texture(0);
        return {texture, 0, 0, colour_channels{0}, rowAlignment};
    }
}