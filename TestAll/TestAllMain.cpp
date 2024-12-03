////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "Debugging/OpenGL/IllegalGPUCallFreeTest.hpp"
#include "Debugging/OpenGL/InfiniteRecursionFreeTest.hpp"
#include "Debugging/OpenGL/MultipleIllegalGPUCallsFreeTest.hpp"
#include "Debugging/OpenGL/NullFunctionPointerFreeTest.hpp"
#include "Graphics/OpenGL/ShaderProgramBrokenStagesFreeTest.hpp"
#include "Graphics/OpenGL/ShaderProgramFileExistenceFreeTest.hpp"
#include "Graphics/OpenGL/ShaderProgramLabellingFreeTest.hpp"
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
            null_function_pointer_free_test{"Null Function Pointer Free Test"},
            illegal_gpu_call_free_test{"Illegal GPU Call Free Test"},
            multiple_illegal_gpu_calls_free_test{"Multiple Illegal GPU Calls Free Test"},
            infinite_recursion_free_test{"Infinite Recursion Free Test"}
        );

        runner.add_test_suite(
            "Shader Program",
            shader_program_file_existence_free_test{"Shader Program File Existence Free Test"},
            shader_program_broken_stages_free_test{"Shader Program Broken Stages Free Test"},
            shader_program_labelling_free_test{"Shader Program Labelling Free Test"}
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

