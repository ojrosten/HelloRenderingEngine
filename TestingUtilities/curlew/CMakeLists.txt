cmake_minimum_required(VERSION 3.20)

include(${BuildSystem}/Utilities.cmake)

set(Dependencies ${CMAKE_CURRENT_LIST_DIR}/../../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
set(Source ${CMAKE_CURRENT_LIST_DIR}/../../Source)

FUNCTION(curlew_glfw_compile_options target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /wd4100 /wd4244)
    else()
        target_compile_options(${target} PRIVATE -Wno-unused-parameter
                                                 -Wno-sign-compare
                                                 -Wno-pedantic
                                                 -Wno-missing-field-initializers
                                                 -Wno-deprecated-declarations)
    endif()
ENDFUNCTION()

sequoia_init()

set(SourceList
    TestFramework/GraphicsTestCore.cpp
    Window/GLFWWrappers.cpp
    Window/RenderingSetup.cpp)

add_library(curlew STATIC ${SourceList})

target_include_directories(curlew PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)

add_subdirectory(${Dependencies}/glfw glfw)
target_link_libraries(curlew PUBLIC glfw)
target_include_directories(curlew PUBLIC ${Dependencies}/glfw/include)
curlew_glfw_compile_options(glfw)

add_subdirectory(${Source}/avocet avocet)
target_link_libraries(curlew PUBLIC avocet)
target_include_directories(curlew PUBLIC ${Source})
target_compile_definitions(curlew PUBLIC EXPOSE_SEQUOIA_BITMASK)

sequoia_finalize_library(curlew)
