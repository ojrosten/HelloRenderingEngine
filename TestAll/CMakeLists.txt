cmake_minimum_required(VERSION 3.20)

project(avocetTests LANGUAGES CXX)

set(Dependencies ../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
set(TestDir ../Tests)
set(TestingUtils ../TestingUtilities)

include(${BuildSystem}/Utilities.cmake)

sequoia_init()

add_executable(TestAll TestAllMain.cpp)

target_sources(TestAll PRIVATE
               ${TestDir}/Graphics/OpenGL/ShaderProgramFreeTest.cpp)

target_include_directories(TestAll PRIVATE ${TestDir})

add_subdirectory(${TestingUtils}/curlew curlew)
target_link_libraries(TestAll PRIVATE curlew)

sequoia_finalize(TestAll ${TestDir} Tests)

