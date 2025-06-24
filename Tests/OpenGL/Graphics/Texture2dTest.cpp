////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "Texture2dTest.hpp"

#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path texture_2d_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void texture_2d_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        /*check_semantics_via_texture_data(
            "Faithful roundtrip rgba and rgb: data sent and received naturally aligned",
            {.image{.data{}, .width{}, .height{}, .num_channels{}, .row_alignment{}}, .decoding{}, .label{}},
            {.image{.data{}, .width{}, .height{}, .num_channels{}, .row_alignment{}}, .decoding{}, .label{}}
        );*/


        // "Faithfully aligned roundtrip rgba and rgb: data is sent and received padded"


        // "Faithful roundtrip red and rg: data sent and received naturally aligned"


        // "Faithfully aligned roundtrip red and rg: data is sent and received padded"


        // "Faithul colours but different paddings: red data is sent with padding and extracted without; rg vice-versa"


        // "Channel Widening extractions: red -> rg with green=0; rgb -> rgba with alpha=255"


        // "Channel Narrowing extractions: rg -> red; rgba -> rgb"


        // "Mixed extractions: red -> rg but with alignment going from 4 -> 2; rgba -> rgb with alignment going from 2 -> 8"
    }
}
