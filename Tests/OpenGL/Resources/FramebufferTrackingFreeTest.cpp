////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "FramebufferTrackingFreeTest.hpp"
#include "avocet/OpenGL/Resources/Framebuffer.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path framebuffer_tracking_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void framebuffer_tracking_free_test::run_tests()
    {
    }
}
