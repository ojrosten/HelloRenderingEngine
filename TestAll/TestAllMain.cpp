////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "Graphics/OpenGL/CommonErrorsFreeTest.hpp"
#include "Graphics/OpenGL/PlatformSpecificErrorsFreeTest.hpp"
#include "Graphics/OpenGL/TargetSpecificErrorsFreeTest.hpp"
#include "Graphics/OpenGL/CommonShaderProgramFreeTest.hpp"
#include "Graphics/OpenGL/TargetSpecificShaderProgramFreeTest.hpp"
#include "sequoia/TestFramework/TestRunner.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        using namespace avocet::testing;
        using namespace std::literals::chrono_literals;

        sequoia::testing::test_runner runner{argc, argv, "Oliver J. Rosten", "    ", {.source_folder{"avocet"}, .additional_dependency_analysis_paths{"TestingUtilities", "dependencies/sequoia/Source"}}};

        runner.add_test_suite(
            "Shader Program",
            common_shader_program_free_test{"Common Shader Program Free Test"},
            target_specific_shader_program_free_test{"Target Specific Shader Program Free Test"}
        );

        runner.add_test_suite(
            "Errors",
            common_errors_free_test{"Common Errors Free Test"},
            platform_specific_errors_free_test{"Platform Specific Errors Free Test"},
            target_specific_errors_free_test{"Target Specific Errors Free Test"}
        );

        runner.execute(sequoia::timer_resolution{1ms});
    }
    catch(const std::exception& e)
    {
        std::cout << e.what();
    }
    catch(...)
    {
        std::cout << "Unrecognized error\n"; 
    }

    return 0;
}

