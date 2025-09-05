////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "Texture2dLabellingTest.hpp"
#include "curlew/Window/GLFWWrappers.hpp"

namespace avocet::testing
{
    namespace agl = avocet::opengl;

    [[nodiscard]]
    std::filesystem::path texture_2d_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void texture_2d_labelling_free_test::labelling_tests(const curlew::window& w)
    {
        std::string label{"This is a nice label!"};
        unique_image image{{1}, 1, 1, colour_channels{1}, alignment{1}};
        agl::texture_2d tex2d{agl::texture_2d_configurator{.data_view{image}, .decoding{}, .parameter_setter{}, .label{label}}};
        check(equality, "", tex2d.extract_label(), label);
    }
}
