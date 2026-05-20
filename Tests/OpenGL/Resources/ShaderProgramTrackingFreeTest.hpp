////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "ResourceTrackingUtilities.hpp"

#include "avocet/OpenGL/Resources/ShaderProgram.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class shader_program_tracking_free_test final : public resource_tracking_test<opengl::shader_program>
    {
    public:
        using resource_tracking_test::resource_tracking_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    };
}
