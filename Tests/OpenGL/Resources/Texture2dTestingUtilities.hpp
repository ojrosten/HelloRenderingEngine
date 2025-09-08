////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "sequoia/TestFramework/MoveOnlyTestCore.hpp"
#include "avocet/OpenGL/Resources/Textures.hpp"

#include "Core/AssetManagement/ImageTestingUtilities.hpp"

namespace sequoia::testing
{
    template<> struct value_tester<avocet::opengl::texture_2d>
    {
        template<test_mode Mode>
        static void test(equivalence_check_t, test_logger<Mode>& logger, const avocet::opengl::texture_2d& texture, const std::optional<avocet::testing::image_data>& prediction)
        {
            if(prediction) {
                const auto imageData{extract_data(texture, avocet::opengl::to_texture_format(prediction->num_channels), prediction->row_alignment)};
                check(equivalence,
                      "Texture Data",
                      logger,
                      imageData,
                      prediction.value());
            }
            else {
                check("Null Texture", logger, texture.is_null());
            }
        }
    };
}
