cmake_minimum_required(VERSION 3.20)

set(BuildSystem ../../dependencies/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)

sequoia_init()

set(SourceList
	Graphics/OpenGL/ShaderProgram.cpp)

add_library(avocet STATIC ${SourceList})

target_include_directories(avocet PUBLIC ../)

add_subdirectory(../../dependencies/glad/OpenGL glad)
target_link_libraries(avocet PUBLIC glad)
target_include_directories(avocet PUBLIC ../dependencies/glad/OpenGL/include)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE CXX)

sequoia_finalize_library(avocet)
