////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "TextureObjectTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path texture_2d_object_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void texture_2d_object_test::run_tests()
    {
        using namespace curlew;
        glfw_manager manager{};
        auto w{manager.create_window({.hiding{window_hiding_mode::on}})};

        using value_type = agl::texture_2d::value_type;
        using opt_data   = std::optional<avocet::testing::texture_data>;

        {
            std::vector<value_type> textureVals{42, 7, 6, 10}, textureVals2{255, 7, 42, 1, 255, 3};
            check_semantics(
                reporter{"Faithful roundtrip rga and rgb"},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals,  1, 1, image_channels{4}, alignment{4}}}},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals2, 2, 1, image_channels{3}, alignment{1}}}},
                opt_data{{textureVals,  1, 1, agl::texture_format::rgba, alignment{4}}},
                opt_data{{textureVals2, 2, 1, agl::texture_format::rgb,  alignment{1}}},
                opt_data{},
                opt_data{{textureVals,  1, 1, agl::texture_format::rgba, alignment{4}}}
            );
        }

        {
            std::vector<value_type> textureVals{42, 7, 6, 10}, textureVals2{255, 7, 42, 0, 1, 255, 3, 0};
            check_semantics(
                reporter{"Faithfully aligned roundtrip rga and rgb"},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals,  1, 1, image_channels{4}, alignment{4}}}},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals2, 1, 2, image_channels{3}, alignment{4}}}},
                opt_data{{textureVals,  1, 1, agl::texture_format::rgba, alignment{4}}},
                opt_data{{textureVals2, 1, 2, agl::texture_format::rgb,  alignment{4}}},
                opt_data{},
                opt_data{{textureVals,  1, 1, agl::texture_format::rgba, alignment{4}}}
            );
        }

        {
            std::vector<value_type> textureVals{42, 7, 6, 0}, textureVals2{255, 7, 42, 1, 255, 3};
            check_semantics(
                reporter{"Faithfully aligned roundtrip red and rg"},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals, 3, 1, image_channels{1}, alignment{2}}}},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals2, 1, 3, image_channels{2}, alignment{2}}}},
                opt_data{{textureVals,  3, 1, agl::texture_format::red, alignment{2}}},
                opt_data{{textureVals2, 1, 3, agl::texture_format::rg, alignment{2}}},
                opt_data{},
                opt_data{{textureVals, 3, 1, agl::texture_format::red, alignment{2}}}
            );
        }

        {
            std::vector<value_type>
                textureVals{42, 7, 6, 0},       textureVals2{255, 7, 42, 0, 1, 255, 3, 0},
                extractedTextureVals{42, 7, 6}, extractedTextureVals2{255, 7, 42, 1, 255, 3};
            check_semantics(
                reporter{"Aligned roundtrip red and rgb"},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals,  3, 1, image_channels{1}, alignment{2}}}},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals2, 1, 2, image_channels{3}, alignment{4}}}},
                opt_data{{extractedTextureVals,  3, 1, agl::texture_format::red, alignment{1}}},
                opt_data{{extractedTextureVals2, 1, 2, agl::texture_format::rgb, alignment{1}}},
                opt_data{},
                opt_data{{extractedTextureVals, 3, 1, agl::texture_format::red, alignment{1}}}
            );
        }

        {
            std::vector<value_type> textureVals{42}, extractedTextureVals{42, 0}, textureVals2{255, 7, 42, 1, 255, 3}, extractedTextureVals2{255, 7, 42, 255, 1, 255, 3, 255};
            check_semantics(
                reporter{"Widening extractions"},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals,  1, 1, image_channels{1}, alignment{1}}}},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals2, 2, 1, image_channels{3}, alignment{1}}}},
                opt_data{{extractedTextureVals,  1, 1,  agl::texture_format::rg,  alignment{1}}},
                opt_data{{extractedTextureVals2, 2, 1, agl::texture_format::rgba, alignment{1}}},
                opt_data{},
                opt_data{{extractedTextureVals,  1, 1, agl::texture_format::rg,   alignment{1}}}
            );
        }

        {
            std::vector<value_type> textureVals{42, 6}, extractedTextureVals{42}, textureVals2{255, 7, 42, 9, 1, 255, 3, 10}, extractedTextureVals2{255, 7, 42, 1, 255, 3};
            check_semantics(
                reporter{"Narrowing extractions"},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals,  1, 1, image_channels{2}, alignment{1}}}},
                agl::texture_2d{agl::texture_2d_configuration{.data{textureVals2, 2, 1, image_channels{4}, alignment{1}}}},
                opt_data{{extractedTextureVals,  1, 1, agl::texture_format::red, alignment{1}}},
                opt_data{{extractedTextureVals2, 2, 1, agl::texture_format::rgb, alignment{1}}},
                opt_data{},
                opt_data{{extractedTextureVals,  1, 1, agl::texture_format::red, alignment{1}}}
            );
        }
    }

    [[nodiscard]]
    std::filesystem::path texture_2d_object_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void texture_2d_object_labelling_free_test::labelling_tests()
    {

        
    }
}
