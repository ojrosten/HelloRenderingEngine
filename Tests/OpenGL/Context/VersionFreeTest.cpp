////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2026.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "VersionFreeTest.hpp"
#include "avocet/OpenGL/Context/Version.hpp"

namespace avocet::testing
{
    [[nodiscard]]
    std::filesystem::path version_free_test::source_file() const
    {
        return std::source_location::current().file_name();
    }

    namespace {
        enum class debug_and_labelling_supported : bool { no, yes };
    }

    void version_free_test::run_tests()
    {
        using namespace opengl;

        auto check_supported{
            [this](opengl_version version, debug_and_labelling_supported prediction, std::source_location loc = std::source_location::current()) {
                check(equality, report({std::format("Debug output support for version {}", version), loc}),  debug_output_supported(version), std::to_underlying(prediction));
                check(equality, report({std::format("Object label support for version {}", version), loc}), object_labels_supported(version), std::to_underlying(prediction));
            }
        };

        check_supported({3, 9}, debug_and_labelling_supported::no);
        check_supported({4, 1}, debug_and_labelling_supported::no);
        check_supported({4, 2}, debug_and_labelling_supported::no);
        check_supported({4, 3}, debug_and_labelling_supported::yes);
        check_supported({4, 6}, debug_and_labelling_supported::yes);
        check_supported({5, 0}, debug_and_labelling_supported::yes);
    }
}
