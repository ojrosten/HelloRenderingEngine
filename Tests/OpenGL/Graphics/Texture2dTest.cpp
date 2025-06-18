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
            {.image{.data{ 42, 7,  6, 10},         .width{1}, .height{1}, .num_channels{4}, .row_alignment{4}}, .decoding{}, .label{}},
            {.image{.data{255, 7, 42,  1, 255, 3}, .width{2}, .height{1}, .num_channels{3}, .row_alignment{1}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithfully aligned roundtrip rgba and rgb: rgb data is sent and received padded",
            {.image{.data{42,  7,  6, 0},               .width{1}, .height{1}, .num_channels{4}, .row_alignment{4}}, .decoding{}, .label{}},
            {.image{.data{255, 7, 42, 0, 1, 255, 3, 0}, .width{1}, .height{2}, .num_channels{3}, .row_alignment{4}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithful roundtrip red and rg: data sent and received naturally aligned",
            {.image{.data{42,  7,  6},            .width{3}, .height{1}, .num_channels{1}, .row_alignment{1}}, .decoding{}, .label{}},
            {.image{.data{255, 7, 42, 1, 255, 3}, .width{1}, .height{3}, .num_channels{2}, .row_alignment{2}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithfully aligned roundtrip red and rg: red data is sent and received padded",
            {.image{.data{42,  7,  6, 0},         .width{3}, .height{1}, .num_channels{1}, .row_alignment{2}}, .decoding{}, .label{}},
            {.image{.data{255, 7, 42, 1, 255, 3}, .width{1}, .height{3}, .num_channels{2}, .row_alignment{2}}, .decoding{}, .label{}}
        );

        check_semantics_via_texture_data(
            "Faithul colours but different paddings: red data is sent with padding and extracted without; rg vice-versa",
            {.image{.data{42,  7,  6, 0},              .width{3}, .height{1}, .num_channels{1}, .row_alignment{2}}, .decoding{}, .label{}},
                   {.data{42,  7,  6},                 .width{3}, .height{1}, .num_channels{1}, .row_alignment{1}},
            {.image{.data{255, 7,       1, 255},       .width{1}, .height{2}, .num_channels{2}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{255, 7, 0, 0, 1, 255, 0, 0}, .width{1}, .height{2}, .num_channels{2}, .row_alignment{4}}
        );

        check_semantics_via_texture_data(
            "Channel Widening extractions: red -> rg with green=0; rgb -> rgba with alpha=255",
            {.image{.data{42},                              .width{1}, .height{1}, .num_channels{1}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{42,  0},                          .width{1}, .height{1}, .num_channels{2}, .row_alignment{1}},
            {.image{.data{255, 7, 42,      1, 255, 3},      .width{2}, .height{1}, .num_channels{3}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{255, 7, 42, 255, 1, 255, 3, 255}, .width{2}, .height{1}, .num_channels{4}, .row_alignment{1}}
        );

        check_semantics_via_texture_data(
            "Channel Widening extractions: rg -> red; rgba -> rgb",
            {.image{.data{42, 6},                        .width{1}, .height{1}, .num_channels{2}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{42},                           .width{1}, .height{1}, .num_channels{1}, .row_alignment{1}},
            {.image{.data{255, 7, 42, 9, 1, 255, 3, 10}, .width{2}, .height{1}, .num_channels{4}, .row_alignment{1}}, .decoding{}, .label{}},
                   {.data{255, 7, 42,    1, 255, 3    }, .width{2}, .height{1}, .num_channels{3}, .row_alignment{1}}
        );

        check_semantics_via_texture_data(
            "Mixed extractions: red -> rg but with alignment going from 4 -> 2; rgba -> rg with alignment going from 2 -> 8",
            {.image{.data{42, 0, 0, 0},                        .width{1}, .height{1}, .num_channels{1}, .row_alignment{4}}, .decoding{}, .label{}},
                   {.data{42, 0},                              .width{1}, .height{1}, .num_channels{2}, .row_alignment{2}},
            {.image{.data{255, 7, 42, 9, 1, 255, 3, 10},       .width{2}, .height{1}, .num_channels{4}, .row_alignment{2}}, .decoding{}, .label{}},
                   {.data{255, 7, 42,    1, 255, 3,    0, 0},  .width{2}, .height{1}, .num_channels{3}, .row_alignment{8}}
        );
    }
}
