////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/OpenGL/Graphics/Textures.hpp"

#include "../../Core/AssetManagement/ImageTestingUtilities.hpp"

namespace agl = avocet::opengl;

namespace avocet::testing {
    struct texture_data {
        using value_type = unsigned char;

        std::vector<value_type> data;
        std::size_t width{}, height{};
        agl::texture_format desired_format{agl::texture_format::rgba};
    };
}

namespace sequoia::testing
{

    template<>
    struct value_tester<agl::texture_2d> {
        using texture_value_type = agl::texture_2d::value_type;
        using texture_data       = avocet::testing::texture_data;

        template<test_mode Mode>
        static void test(equivalence_check_t,
            test_logger<Mode>& logger,
            const agl::texture_2d& texture,
            const std::optional<texture_data>& prediction)
        {
            if(prediction) {
                const auto imageData{extract_image(texture, prediction.value().desired_format)};
                check(equality,
                      "Texture Data",
                      logger,
                      imageData.span(),
                      std::span<const texture_value_type>(prediction.value().data));
            }
            else {
                check("Null Buffer", logger, texture.is_null());
            }
        }
    };
}
