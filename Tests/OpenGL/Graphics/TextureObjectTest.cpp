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
        using opt_data = std::optional<avocet::image_view>;

        std::vector<value_type> textureVals{42, 7, 6, 10}, textureVals2{255, 0, 0, 4, 0, 255, 0, 8};
        check_semantics(
            reporter{""},
            agl::texture_2d{agl::texture_2d_configuration{.data{textureVals, 1, 1, 4}}},
            agl::texture_2d{agl::texture_2d_configuration{.data{textureVals2, 2, 1, 4}}},
            opt_data{avocet::image_view{textureVals, 1, 1, 4}},
            opt_data{avocet::image_view{textureVals2, 2, 1, 4}},
            opt_data{},
            opt_data{avocet::image_view{textureVals, 1, 1, 4}}
        );
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
