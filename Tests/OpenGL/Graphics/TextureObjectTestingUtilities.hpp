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

namespace sequoia::testing
{
    namespace agl = avocet::opengl;

    template<>
    struct value_tester<agl::texture_2d> {
        using texture_value_type = agl::texture_2d::value_type;

        template<test_mode Mode>
        static void test(equivalence_check_t,
            test_logger<Mode>& logger,
            const agl::texture_2d& texture,
            const std::optional<avocet::image_view>& prediction)
        {
            if(prediction) {
                const auto imageData{extract_image(texture, agl::texture_format::rgba)};
                check(equality,
                     "Texture Data",
                     logger,
                     std::span<const texture_value_type>{imageData.data},
                     prediction.value().span());
            }
            else {
                check("Null Buffer", logger, texture.is_null());
            }
        }
    };
}
