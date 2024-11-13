cmake_minimum_required(VERSION 3.20)

set(Dependencies ${CMAKE_CURRENT_LIST_DIR}/../../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)
set(Source ${CMAKE_CURRENT_LIST_DIR}/../../Source)

sequoia_init()

set(SourceList
    TestFramework/GraphicsTestCore.cpp
    Window/GLFWWrappers.cpp)

add_library(curlew STATIC ${SourceList})

target_include_directories(curlew PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)

add_subdirectory(${Dependencies}/glfw glfw)
target_link_libraries(curlew PUBLIC glfw)
target_include_directories(curlew PUBLIC ${Dependencies}/glfw/include)

add_subdirectory(${Source}/avocet avocet)
target_link_libraries(curlew PUBLIC avocet)
target_include_directories(curlew PUBLIC ${Source})

sequoia_finalize_library(curlew)
