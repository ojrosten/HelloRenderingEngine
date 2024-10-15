cmake_minimum_required(VERSION 3.20)

project(RenderingEngineDemo LANGUAGES C CXX)

set(Dependencies ../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
set(DemoDir ../Demo)
set(Source ../Source)

include(${BuildSystem}/Utilities.cmake)

sequoia_init()

add_executable(Demo DemoMain.cpp)

target_sources(Demo PRIVATE TestingUtilities/GLFWWrappers.cpp)

target_include_directories(Demo PRIVATE ${DemoDir})

add_subdirectory(${Dependencies}/glfw glfw)
target_link_libraries(Demo PRIVATE glfw)
target_include_directories(Demo PRIVATE ${Dependencies}/glfw/include)

add_subdirectory(${Source}/avocet avocet)
target_link_libraries(Demo PRIVATE avocet)
target_include_directories(Demo PRIVATE ${Source})

sequoia_finalize_executable(Demo)

