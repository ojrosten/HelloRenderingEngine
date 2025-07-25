////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

#pragma once

/*! \file */

#include "BufferObjectTestingUtilities.hpp"
#include "curlew/TestFramework/GraphicsLabellingTestCore.hpp"

namespace avocet::testing
{
    using namespace sequoia::testing;

    class buffer_object_test final : public move_only_test
    {
    public:
        using parallelizable_type = std::false_type;

        using move_only_test::move_only_test;

        [[nodiscard]]
        std::filesystem::path source_file() const;

        void run_tests();
    private:
        template<class Buffer>
            requires is_gl_buffer_v<Buffer>
        void execute();
    };
}
