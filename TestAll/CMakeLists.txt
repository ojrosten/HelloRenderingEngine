cmake_minimum_required(VERSION 3.20)

project(avocetTests LANGUAGES C CXX)

set(Dependencies ${CMAKE_CURRENT_LIST_DIR}/../dependencies)
set(BuildSystem  ${Dependencies}/sequoia/build_system)
set(TestDir      ${CMAKE_CURRENT_LIST_DIR}/../Tests)
set(TestingUtils ${CMAKE_CURRENT_LIST_DIR}/../TestingUtilities)

include(${BuildSystem}/Utilities.cmake)

sequoia_init()

add_executable(TestAll TestAllMain.cpp)

target_sources(TestAll PRIVATE
               ${TestDir}/Core/AssetManagement/ImageTest.cpp
               ${TestDir}/Core/AssetManagement/ImageTestingDiagnostics.cpp
               ${TestDir}/Core/AssetManagement/ImageTestingUtilities.cpp
               ${TestDir}/OpenGL/Debugging/IllegalGPUCallFreeTest.cpp
               ${TestDir}/OpenGL/Debugging/InfiniteLoopFreeTest.cpp
               ${TestDir}/OpenGL/Debugging/MultipleIllegalGPUCallsFreeTest.cpp
               ${TestDir}/OpenGL/Debugging/NullFunctionPointerFreeTest.cpp
               ${TestDir}/OpenGL/Graphics/BufferObjectTest.cpp
               ${TestDir}/OpenGL/Graphics/BufferObjectTestingDiagnostics.cpp
               ${TestDir}/OpenGL/Graphics/ResourceHandleTest.cpp
               ${TestDir}/OpenGL/Graphics/ResourceHandleTestingDiagnostics.cpp
               ${TestDir}/OpenGL/Graphics/ShaderProgramBrokenStagesFreeTest.cpp
               ${TestDir}/OpenGL/Graphics/ShaderProgramFileExistenceFreeTest.cpp
               ${TestDir}/OpenGL/Graphics/ShaderProgramLabellingFreeTest.cpp
               ${TestDir}/TestFramework/OpenGL/LabellingTestDiagnostics.cpp)

target_include_directories(TestAll PRIVATE ${TestDir})

add_subdirectory(${TestingUtils}/curlew curlew)
target_link_libraries(TestAll PRIVATE curlew)

sequoia_finalize_tests(TestAll ${TestDir} Tests)
