cmake_minimum_required(VERSION 3.20)

project(RenderingEngineDemo LANGUAGES C CXX)

set(Dependencies ${CMAKE_CURRENT_LIST_DIR}/../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
set(DemoDir ${CMAKE_CURRENT_LIST_DIR}/../Demo)
set(TestingUtils ${CMAKE_CURRENT_LIST_DIR}/../TestingUtilities)

include(${BuildSystem}/Utilities.cmake)

sequoia_init()

add_executable(Demo
               DemoMain.cpp
               Examples/PonyPolygons.cpp
)

target_include_directories(Demo PRIVATE ${DemoDir})

add_subdirectory(${TestingUtils}/curlew curlew)
target_link_libraries(Demo PRIVATE curlew)

sequoia_finalize_executable(Demo)