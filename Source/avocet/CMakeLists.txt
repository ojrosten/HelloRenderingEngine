cmake_minimum_required(VERSION 3.20)

set(Dependencies ${CMAKE_CURRENT_LIST_DIR}/../../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)

sequoia_init()

set(SourceList
    Core/AssetManagement/Image.cpp
    OpenGL/Debugging/Errors.cpp
    OpenGL/Graphics/ShaderProgram.cpp
    OpenGL/Graphics/Textures.cpp
    OpenGL/Utilities/Version.cpp)

add_library(avocet STATIC ${SourceList})

target_include_directories(avocet PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)

add_subdirectory(${Dependencies}/glad/OpenGL glad)
target_link_libraries(avocet PUBLIC glad)
target_include_directories(avocet PUBLIC ${Dependencies}/glad/OpenGL/include)

add_subdirectory(${Dependencies}/sequoia/Source/sequoia sequoia)
target_link_libraries(avocet PUBLIC sequoia)

target_include_directories(avocet PRIVATE ${Dependencies}/stb)

sequoia_finalize_library(avocet)
