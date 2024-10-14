cmake_minimum_required(VERSION 3.20)

project(avocetTests LANGUAGES CXX)

set(BuildSystem ../dependencies/sequoia/build_system)
set(TestDir ../Tests)
set(SourceDir ../Source)

include(${BuildSystem}/Utilities.cmake)

sequoia_init()

add_executable(TestAll TestAllMain.cpp)

#!add_subdirectory(${SourceDir}/avocet avocet)
#!target_link_libraries(TestAll PUBLIC avocet)

target_sources(TestAll PRIVATE)

target_include_directories(TestAll PRIVATE ${SourceDir})
target_include_directories(TestAll PRIVATE ${TestDir})

sequoia_finalize(TestAll ${TestDir} Tests)

