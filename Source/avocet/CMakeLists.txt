cmake_minimum_required(VERSION 3.20)

set(BuildSystem ../../dependencies/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)

sequoia_init()

set(SourceList
	Graphics/OpenGL/ShaderProgram.cpp)

add_library(avocet STATIC ${SourceList})

target_include_directories(avocet PUBLIC ../)

sequoia_finalize_library(avocet)
