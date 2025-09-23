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
            w,
            {.image{.data{42, 4, 7, 3},       .width{1}, .height{1}, .num_channels{4}, .row_alignment{4}}, .decoding{}, .label{}},
            {.image{.data{42, 4, 7, 2, 6, 8}, .width{2}, .height{1}, .num_channels{3}, .row_alignment{1}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithfully aligned roundtrip rgba and rgb: data is sent and received padded",
            w,
            {.image{.data{42, 4, 7, 3, 0, 0, 0, 0}, .width{1}, .height{1}, .num_channels{4}, .row_alignment{8}}, .decoding{}, .label{}},
            {.image{.data{42, 4, 7, 2, 6, 8, 0, 0}, .width{2}, .height{1}, .num_channels{3}, .row_alignment{4}}, .decoding{}, .label{}}
        );


        check_semantics_via_texture_data(
            "Faithful roundtrip red and rg: data sent and received naturally aligned",
            w,
            {.image{.data{42, 7, 10},         .width{3}, .height{1}, .num_channels{1}, .row_alignment{1}}, .decoding{}, .label{}},
            {.image{.data{31, 9, 2, 6, 1, 8}, .width{1}, .height{3}, .num_channels{2}, .row_alignment{2}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithfully aligned roundtrip red and rg: data is sent and received padded",
            w,
            {.image{.data{42, 7, 10, 0},             .width{3}, .height{1}, .num_channels{1}, .row_alignment{4}}, .decoding{}, .label{}},
            {.image{.data{31, 9,  0, 0, 2, 6, 0, 0}, .width{1}, .height{2}, .num_channels{2}, .row_alignment{4}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithful colours but different paddings: red data is sent with padding and extracted without; rg vice-versa",
            w,
            {.image{.data{42, 0, 0, 0},             .width{1}, .height{1}, .num_channels{1}, .row_alignment{4}}, .decoding{}, .label{}},
                   {.data{42},                      .width{1}, .height{1}, .num_channels{1}, .row_alignment{1}},
            {.image{.data{42, 4, 6, 8},             .width{1}, .height{2}, .num_channels{2}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{42, 4, 0, 0, 6, 8, 0, 0}, .width{1}, .height{2}, .num_channels{2}, .row_alignment{4}}
        );

        check_semantics_via_texture_data(
            "Channel Widening extractions: red -> rg with green=0; rgb -> rgba with alpha=255",
            w,
            {.image{.data{42, 7, 10},                   .width{3}, .height{1}, .num_channels{1}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{42, 0, 7, 0, 10, 0},          .width{3}, .height{1}, .num_channels{2}, .row_alignment{1}},
            {.image{.data{31, 9, 2, 6, 4, 5},           .width{1}, .height{2}, .num_channels{3}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{31, 9, 2, 255, 6, 4, 5, 255}, .width{1}, .height{2}, .num_channels{4}, .row_alignment{4}}
        );

        check_semantics_via_texture_data(
            "Channel Narrowing extractions: rg -> red; rgba -> rgb",
            w,
            {.image{.data{42, 7, 10, 2},              .width{2}, .height{1}, .num_channels{2}, .row_alignment{2}}, .decoding{}, .label{}},
                   {.data{42,    10   },              .width{2}, .height{1}, .num_channels{1}, .row_alignment{1}},
            {.image{.data{31, 9, 2, 13, 6, 4, 5, 12}, .width{1}, .height{2}, .num_channels{4}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{31, 9, 2,     6, 4, 5    }, .width{1}, .height{2}, .num_channels{3}, .row_alignment{1}}
        );

        check_semantics_via_texture_data(
            "Mixed extractions: red -> rg but with alignment going from 4 -> 2; rgba -> rgb with alignment going from 2 -> 8",
            w,
            {.image{.data{42, 7, 0, 0},                    .width{2}, .height{1}, .num_channels{1}, .row_alignment{4}}, .decoding{}, .label{}},
                   {.data{42, 0, 7, 0},                    .width{2}, .height{1}, .num_channels{2}, .row_alignment{2}},
            {.image{.data{31, 9, 2, 13, 6, 4, 5, 12},      .width{2}, .height{1}, .num_channels{4}, .row_alignment{2}}, .decoding{}, .label{}},
                   {.data{31, 9, 2,     6, 4, 5,    0, 0}, .width{2}, .height{1}, .num_channels{3}, .row_alignment{8}}
        );
    }
}
