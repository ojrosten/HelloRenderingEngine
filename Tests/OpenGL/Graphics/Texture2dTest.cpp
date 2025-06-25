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

        check_semantics_via_texture_data(
            "Faithful roundtrip rgba and rgb: data sent and received naturally aligned",
            {.image{.data{42, 6, 3, 7},       .width{1}, .height{1}, .num_channels{4}, .row_alignment{4}}, .decoding{}, .label{}},
            {.image{.data{42, 6, 3, 8, 9, 1}, .width{2}, .height{1}, .num_channels{3}, .row_alignment{1}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithfully aligned roundtrip rgba and rgb: data is sent and received padded",
            {.image{.data{42, 6, 3, 7, 0, 0, 0, 0}, .width{1}, .height{1}, .num_channels{4}, .row_alignment{8}}, .decoding{}, .label{}},
            {.image{.data{42, 6, 3, 8, 9, 1, 0, 0}, .width{2}, .height{1}, .num_channels{3}, .row_alignment{4}}, .decoding{}, .label{}}
        );


        // "Faithful roundtrip red and rg: data sent and received naturally aligned"


        // "Faithfully aligned roundtrip red and rg: data is sent and received padded"


        // "Faithul colours but different paddings: red data is sent with padding and extracted without; rg vice-versa"
        check_semantics_via_texture_data(
             "Faithul colours but different paddings: red data is sent with padding and extracted without; rg vice-versa",
            {.image{.data{42, 0, 0, 0},             .width{1}, .height{1}, .num_channels{1}, .row_alignment{4}}, .decoding{}, .label{}},
                   {.data{42},                      .width{1}, .height{1}, .num_channels{1}, .row_alignment{1}},
            {.image{.data{42, 6, 0, 0, 9, 1, 0, 0}, .width{1}, .height{2}, .num_channels{2}, .row_alignment{4}}, .decoding{}, .label{}},
                   {.data{42, 6,       9, 1},       .width{1}, .height{2}, .num_channels{2}, .row_alignment{1}}
        );

        // "Channel Widening extractions: red -> rg with green=0; rgb -> rgba with alpha=255"


        // "Channel Narrowing extractions: rg -> red; rgba -> rgb"


        // "Mixed extractions: red -> rg but with alignment going from 4 -> 2; rgba -> rgb with alignment going from 2 -> 8"
    }
}
