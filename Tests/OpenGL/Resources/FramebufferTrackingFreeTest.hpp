////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "ResourceTrackingUtilities.hpp"

#include "avocet/OpenGL/Resources/Framebuffer.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class framebuffer_tracking_free_test final : public resource_tracking_test<opengl::framebuffer_object>
    {
    public:
        using resource_tracking_test<opengl::framebuffer_object>::resource_tracking_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    };
}
