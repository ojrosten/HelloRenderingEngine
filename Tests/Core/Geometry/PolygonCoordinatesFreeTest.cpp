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

        const float x1{-0.5f * std::sin(2.0f * std::numbers::pi_v<float> / 3.0f)};
        const float y1{ 0.5f * std::cos(2.0f * std::numbers::pi_v<float> / 3.0f)};
        const float x2{-0.5f * std::sin(4.0f * std::numbers::pi_v<float> / 3.0f)};
        const float y2{ 0.5f * std::cos(4.0f * std::numbers::pi_v<float> / 3.0f)};
        check(equality, "", poly_maker_t{}(), verts_t{{{{0.0f, 0.5f}}, {{x1, y1}}, {{x2, y2}}}});
    }
}
