////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "Texture2dTestingUtilities.hpp"
#include "curlew/TestFramework/GraphicsLabellingTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class texture_2d_labelling_free_test final : public curlew::graphics_labelling_test
    {
    public:
        using curlew::graphics_labelling_test::graphics_labelling_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void labelling_tests();
    };
}
