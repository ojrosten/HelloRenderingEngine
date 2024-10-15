cmake_minimum_required(VERSION 3.20)

project(avocetTests LANGUAGES CXX)

set(Dependencies ../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
set(TestDir ../Tests)
set(Source ../Source)

include(${BuildSystem}/Utilities.cmake)

sequoia_init()

add_executable(TestAll TestAllMain.cpp)

target_sources(TestAll PRIVATE
               ${TestDir}/Graphics/OpenGL/ShaderProgramFreeTest.cpp)

add_subdirectory(${Dependencies}/glfw glfw)
target_link_libraries(TestAll PRIVATE glfw)
target_include_directories(TestAll PRIVATE ${Dependencies}/glfw/include)

add_subdirectory(${Source}/avocet avocet)
target_link_libraries(TestAll PRIVATE avocet)

target_include_directories(TestAll PRIVATE ${Source})
target_include_directories(TestAll PRIVATE ${TestDir})

sequoia_finalize(TestAll ${TestDir} Tests)

