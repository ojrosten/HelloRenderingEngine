////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "BuffersLabellingFreeTest.hpp"
#include "avocet/Graphics/OpenGL/Buffers.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path buffers_labelling_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void buffers_labelling_free_test::labelling_tests()
    {
        namespace agl = avocet::opengl;
    }
}
