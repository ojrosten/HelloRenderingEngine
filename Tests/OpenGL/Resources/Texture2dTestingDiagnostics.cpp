////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "Texture2dTestingDiagnostics.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path texture_2d_false_negative_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void texture_2d_false_negative_test::run_tests()
    {
        using namespace curlew;

        auto w{create_window({.hiding{window_hiding_mode::on}})};

        using value_type = agl::texture_2d::value_type;
        using opt_data   = std::optional<image_data>;
        std::vector<value_type> textureVals{ 255, 255, 255,  1},
                                textureVals2{42,    7,   6, 10},
                                textureVals3{255,   0,   0,  0, 255, 0};
        unique_image image{textureVals, 1, 1, colour_channels{4}, alignment{1}};

        agl::texture_2d tex2d{w.context(), agl::texture_2d_configurator{.common_config{.decoding{}, .parameter_setter{}, .label{}}, .data_view{image}}};

        check(equivalence, "Texture which should be null", tex2d, opt_data{});
        check(equivalence, "Empty texture",                tex2d, opt_data{{.data{},             .width{},  .height{},  .num_channels{1}, .row_alignment{1}}});
        check(equivalence, "Incorrect texture data",       tex2d, opt_data{{.data{textureVals2}, .width{1}, .height{1}, .num_channels{4}, .row_alignment{1}}});
        check(equivalence, "Mismatched textures",          tex2d, opt_data{{.data{textureVals3}, .width{2}, .height{1}, .num_channels{3}, .row_alignment{1}}});
        check(equivalence, "Mismatched padding",           tex2d, opt_data{{.data{textureVals},  .width{1}, .height{1}, .num_channels{4}, .row_alignment{8}}});
    }
}
