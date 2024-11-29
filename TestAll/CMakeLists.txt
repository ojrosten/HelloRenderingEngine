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
               ${TestDir}/Debugging/OpenGL/ErrorsBuildSelectiveTargetSpecificFreeTest.cpp
               ${TestDir}/Debugging/OpenGL/ErrorsCommonFreeTest.cpp
               ${TestDir}/Graphics/OpenGL/ShaderProgramBuildAndVersionSelectiveFreeTest.cpp
               ${TestDir}/Graphics/OpenGL/ShaderProgramCommonFreeTest.cpp
               ${TestDir}/Graphics/OpenGL/ShaderProgramTargetSpecificFreeTest.cpp)

target_include_directories(TestAll PRIVATE ${TestDir})

add_subdirectory(${TestingUtils}/curlew curlew)
target_link_libraries(TestAll PRIVATE curlew)

sequoia_finalize_tests(TestAll ${TestDir} Tests)

