cmake_minimum_required(VERSION 3.20)

set(Dependencies ../../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)

sequoia_init()

set(SourceList
	Graphics/OpenGL/ShaderProgram.cpp)

add_library(avocet STATIC ${SourceList})

target_include_directories(avocet PUBLIC ../)

add_subdirectory(${Dependencies}/glad/OpenGL glad)
target_link_libraries(avocet PUBLIC glad)
target_include_directories(avocet PUBLIC ${Dependencies}/glad/OpenGL/include)

sequoia_finalize_library(avocet)
