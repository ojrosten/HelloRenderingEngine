////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "Debugging/OpenGL/ErrorsBuildSelectiveTargetSpecificFreeTest.hpp"
#include "Debugging/OpenGL/ErrorsCommonFreeTest.hpp"
#include "Graphics/OpenGL/ShaderProgramBuildAndVersionSelectiveFreeTest.hpp"
#include "Graphics/OpenGL/ShaderProgramCommonFreeTest.hpp"
#include "Graphics/OpenGL/ShaderProgramTargetSpecificFreeTest.hpp"
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
            "Errors",
            errors_common_free_test{"Errors Common Free Test"},
            errors_build_selective_target_specific_free_test{"Errors Build Selective Target Specific Free Test"}
        );

        runner.add_test_suite(
            "Shader Program",
            shader_program_common_free_test{"Shader Program Common Free Test"},
            shader_program_target_specific_free_test{"Shader Program Target Specific Free Test"},
            shader_program_build_and_version_selective_free_test{"Shader Program Build And Version Selective Free Test"}
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

