////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "curlew/TestFramework/GraphicsTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class shader_program_build_and_version_selective_free_test final : public curlew::build_and_version_selective_graphics_test
    {
    public:
        using curlew::build_and_version_selective_graphics_test::build_and_version_selective_graphics_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();

        void labelling_tests();
    };
}
