////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2025.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "UniqueImageThreadingFreeTest.hpp"
#include "ImageTestingUtilities.hpp"
#include "avocet/Core/AssetManagement/Image.hpp"
#include "sequoia/Core/ContainerUtilities/ArrayUtilities.hpp"

#include <future>
#include <latch>
#include <thread>

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path unique_image_threading_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void unique_image_threading_free_test::run_tests()
    {
    }
}
