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

    class common_shader_program_free_test final : public curlew::graphics_test
    {
    public:
        using curlew::graphics_test::graphics_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    };
}