cmake_minimum_required(VERSION 3.20)

set(Dependencies ${CMAKE_CURRENT_LIST_DIR}/../../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)

sequoia_init()

set(SourceList
    Core/AssetManagement/Image.cpp
    Core/Formatting/Formatting.cpp
    Core/Geometry/Viewport.cpp
    OpenGL/Capabilities/Capabilities.cpp
    OpenGL/Capabilities/CapabilitiesConfiguration.cpp
    OpenGL/Context/CharacteristicContext.cpp
    OpenGL/Context/Context.cpp
    OpenGL/Context/ContextBase.cpp
    OpenGL/Debugging/Errors.cpp
    OpenGL/ResourceInfrastructure/Labels.cpp
    OpenGL/Resources/Framebuffer.cpp
    OpenGL/Resources/ShaderProgram.cpp
    OpenGL/Resources/Textures.cpp
    OpenGL/Utilities/Messages.cpp)

add_library(avocet STATIC ${SourceList})

target_include_directories(avocet PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)

add_subdirectory(${Dependencies}/glad/OpenGL glad)
target_link_libraries(avocet PUBLIC glad)
target_include_directories(avocet PUBLIC ${Dependencies}/glad/OpenGL/include)

add_subdirectory(${Dependencies}/sequoia/Source/sequoia sequoia)
target_link_libraries(avocet PUBLIC sequoia)

target_include_directories(avocet PRIVATE ${Dependencies}/stb)

sequoia_finalize_library(avocet)
