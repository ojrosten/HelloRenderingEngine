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
               ${TestDir}/Debugging/OpenGL/IllegalGPUCallFreeTest.cpp
               ${TestDir}/Debugging/OpenGL/InfiniteLoopFreeTest.cpp
               ${TestDir}/Debugging/OpenGL/MultipleIllegalGPUCallsFreeTest.cpp
               ${TestDir}/Debugging/OpenGL/NullFunctionPointerFreeTest.cpp
               ${TestDir}/Graphics/OpenGL/ResourceHandleTest.cpp
               ${TestDir}/Graphics/OpenGL/ResourceHandleTestingDiagnostics.cpp
               ${TestDir}/Graphics/OpenGL/ShaderProgramBrokenStagesFreeTest.cpp
               ${TestDir}/Graphics/OpenGL/ShaderProgramFileExistenceFreeTest.cpp
               ${TestDir}/Graphics/OpenGL/ShaderProgramLabellingFreeTest.cpp
               ${TestDir}/TestFramework/OpenGL/LabellingTestDiagnostics.cpp)

target_include_directories(TestAll PRIVATE ${TestDir})

add_subdirectory(${TestingUtils}/curlew curlew)
target_link_libraries(TestAll PRIVATE curlew)

sequoia_finalize_tests(TestAll ${TestDir} Tests)
