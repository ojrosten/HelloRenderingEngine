cmake_minimum_required(VERSION 3.20)

set(Dependencies ../../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)

sequoia_init()

set(SourceList
	Window/GLFWWrappers.cpp)

add_library(curlew STATIC ${SourceList})

target_include_directories(curlew PUBLIC ../)

add_subdirectory(${Dependencies}/glfw glfw)
target_link_libraries(curlew PRIVATE glfw)
target_include_directories(curlew PRIVATE ${Dependencies}/glfw/include)

sequoia_finalize_library(curlew)
