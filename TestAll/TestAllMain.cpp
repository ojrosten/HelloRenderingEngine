////////////////////////////////////////////////////////////////////
//                Copyright Oliver J. Rosten 2024.                //
// Distributed under the GNU GENERAL PUBLIC LICENSE, Version 3.0. //
//    (See accompanying file LICENSE.md or copy at                //
//          https://www.gnu.org/licenses/gpl-3.0.en.html)         //
////////////////////////////////////////////////////////////////////

/*! \file */

#include "Core/AssetManagement/AlignmentTest.hpp"
#include "Core/AssetManagement/AlignmentTestingDiagnostics.hpp"
#include "Core/AssetManagement/ColourChannelsTest.hpp"
#include "Core/AssetManagement/ColourChannelsTestingDiagnostics.hpp"
#include "Core/AssetManagement/ImageViewTest.hpp"
#include "Core/AssetManagement/ImageViewTestingDiagnostics.hpp"
#include "Core/AssetManagement/PaddedRowSizeFreeTest.hpp"
#include "Core/AssetManagement/UniqueImageTest.hpp"
#include "Core/AssetManagement/UniqueImageTestingDiagnostics.hpp"
#include "OpenGL/Debugging/IllegalGPUCallFreeTest.hpp"
#include "OpenGL/Debugging/InfiniteLoopFreeTest.hpp"
#include "OpenGL/Debugging/MultipleIllegalGPUCallsFreeTest.hpp"
#include "OpenGL/Debugging/NullFunctionPointerFreeTest.hpp"
#include "OpenGL/Graphics/BufferObjectTest.hpp"
#include "OpenGL/Graphics/BufferObjectTestingDiagnostics.hpp"
#include "OpenGL/Graphics/ResourceHandleTest.hpp"
#include "OpenGL/Graphics/ResourceHandleTestingDiagnostics.hpp"
#include "OpenGL/Graphics/ShaderProgramBrokenStagesFreeTest.hpp"
#include "OpenGL/Graphics/ShaderProgramFileExistenceFreeTest.hpp"
#include "OpenGL/Graphics/ShaderProgramLabellingFreeTest.hpp"
#include "TestFramework/OpenGL/LabellingTestDiagnostics.hpp"
#include "sequoia/TestFramework/TestRunner.hpp"
#include <iostream>

int main(int argc, char** argv)
{
    try
    {
        using namespace avocet::testing;
        using namespace std::literals::chrono_literals;

        std::cout << curlew::rendering_setup_summary();

        sequoia::testing::test_runner runner{argc, argv, "Oliver J. Rosten", "    ", {.source_folder{"avocet"}, .additional_dependency_analysis_paths{"TestingUtilities", "dependencies/sequoia/Source"}}};
        sequoia::write_to_file(runner.proj_paths().executable().parent_path() / "RenderingSetup.txt", curlew::rendering_setup_discriminator(curlew::full_specificity()));

        runner.add_test_suite(
            "Test Framework",
            labelling_false_negative_test{"Labelling False Negative Test"}
        );

        runner.add_test_suite(
            "Errors",
            null_function_pointer_free_test{"Null Function Pointer Free Test"},
            illegal_gpu_call_free_test{"Illegal GPU Call Free Test"},
            multiple_illegal_gpu_calls_free_test{"Multiple Illegal GPU Calls Free Test"},
            infinite_loop_free_test{"Infinite Loop Free Test"}
        );

        runner.add_test_suite(
            "Resource Handle",
            resource_handle_false_negative_test{"False Negative Test"},
            resource_handle_test{"Unit Test"}
        );

        runner.add_test_suite(
            "Shader Program",
            shader_program_file_existence_free_test{"Shader Program File Existence Free Test"},
            shader_program_broken_stages_free_test{"Shader Program Broken Stages Free Test"},
            shader_program_labelling_free_test{"Shader Program Labelling Free Test"}
        );

        runner.add_test_suite(
            "Buffer Object",
            buffer_object_false_negative_test{"False Negative Test"},
            buffer_object_test{"Semantics Test"},
            buffer_object_labelling_free_test{"Labelling Test"}
        );

        runner.add_test_suite(
            "Image",
            colour_channels_false_negative_test{"False Negative Test"},
            colour_channels_test{"Unit Test"},
            alignment_false_negative_test{"False Negative Test"},
            alignment_test{"Unit Test"},
            padded_row_size_free_test{"Padded Row Size Free Test"},
            image_false_negative_test{"False Negative Test"},
            unique_image_test{"Unit Test"},
            image_view_false_negative_test{"False Negative Test"},
            image_view_test{"Unit Test"}
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

