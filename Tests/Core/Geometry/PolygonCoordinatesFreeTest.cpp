////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "PolygonCoordinatesFreeTest.hpp"
#include "avocet/Core/Geometry/PolygonCoordinates.hpp"

// TO DO: These need to be made more naturally available from sequoia
#include "sequoia/../../Tests/Maths/Geometry/GeometryTestingUtilities.hpp"
#include "sequoia/../../Tests/Core/DataStructures/MemOrderedTupleTestingUtilities.hpp"

#include <ranges>

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path polygon_coordinates_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    void polygon_coordinates_free_test::run_tests()
    {
        using poly_maker_t = make_polygon<float, 3, dimensionality{2}>;
        using verts_t      = poly_maker_t::vertices_type;

        const float xHigh{std::sqrt(3.0f) / 4.0f};
        for(auto [obtained, predicted] : std::views::zip(poly_maker_t{}(), verts_t{{{{0.0f, 0.5f}}, {{-xHigh, -0.25f}}, {{xHigh, -0.25f}}}})) {
            check(within_tolerance{1e-6f}, "", sequoia::get<0>(obtained), sequoia::get<0>(predicted));
        }
    }
}
