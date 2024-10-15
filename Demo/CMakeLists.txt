cmake_minimum_required(VERSION 3.20)

project(RenderingEngineDemo LANGUAGES C CXX)

set(Dependencies ../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
set(DemoDir ../Demo)
set(TestingUtils ../TestingUtilities)

include(${BuildSystem}/Utilities.cmake)

sequoia_init()

add_executable(Demo DemoMain.cpp)

target_include_directories(Demo PRIVATE ${DemoDir})

add_subdirectory(${TestingUtils}/curlew curlew)
target_link_libraries(Demo PRIVATE curlew)

sequoia_finalize_executable(Demo)