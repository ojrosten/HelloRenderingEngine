////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "FramebufferFreeTest.hpp"
#include "avocet/OpenGL/Resources/Framebuffer.hpp"

#include "Core/AssetManagement/ImageTestingUtilities.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path framebuffer_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void framebuffer_free_test::run_tests()
    {
        using namespace curlew;
        using namespace opengl;

        auto w{create_window({.extent{.width{1}, .height{1}}, .hiding{window_hiding_mode::on}})};

        framebuffer_object
            fbo{
                w.context(),
                fbo_configurator{.label{}}
            };
    }
}
