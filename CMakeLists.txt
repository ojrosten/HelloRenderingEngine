cmake_minimum_required(VERSION 3.24)


project(RenderingEngine LANGUAGES C CXX)

set(Dependencies ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
set(DemoDir ${CMAKE_CURRENT_SOURCE_DIR}/Demo)
set(TestingUtils ${CMAKE_CURRENT_SOURCE_DIR}/TestingUtilities)

include(${BuildSystem}/Utilities.cmake)

##TODO: Needed for anythign? sequoia_init()

add_subdirectory(${TestingUtils}/curlew curlew)

add_subdirectory(Demo)

