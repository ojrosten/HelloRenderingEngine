////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "PaddedRowSizeFreeTest.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path padded_row_size_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void padded_row_size_free_test::run_tests()
    {
    }
}
