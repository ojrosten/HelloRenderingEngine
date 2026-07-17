cmake_minimum_required(VERSION 3.20)

set(Dependencies ${CMAKE_CURRENT_LIST_DIR}/../../dependencies)
set(BuildSystem ${Dependencies}/sequoia/build_system)
include(${BuildSystem}/Utilities.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

sequoia_init()

set(SourceList
    Core/AssetManagement/Image.cpp
    Core/Formatting/Formatting.cpp
    Core/Geometry/Viewport.cpp
    Core/ShaderCompilation/ShaderCompiler.cpp
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
    OpenGL/Utilities/Messages.cpp
    Vulkan/Presentation/Presentation.cpp
    Vulkan/Common/Conversions.cpp
    Vulkan/Common/VulkanConfig.cpp
    Vulkan/Rendering/RenderingSystem.cpp
    Vulkan/Setup/Devices.cpp
    Vulkan/Setup/Instance.cpp
    Vulkan/Shaders/Shaders.cpp
)

add_library(avocet STATIC ${SourceList})

target_include_directories(avocet PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../)

add_subdirectory(${Dependencies}/glad/OpenGL glad)
target_link_libraries(avocet PUBLIC glad)
target_include_directories(avocet PUBLIC ${Dependencies}/glad/OpenGL/include)

add_subdirectory(${Dependencies}/sequoia/Source/sequoia sequoia)
target_link_libraries(avocet PUBLIC sequoia)

target_include_directories(avocet PRIVATE ${Dependencies}/stb)

find_package(Vulkan REQUIRED)
add_subdirectory(${Dependencies}/volk volk)
target_link_libraries(avocet PUBLIC volk)
target_include_directories(avocet PUBLIC ${Vulkan_INCLUDE_DIR})

set(SHADERC_SKIP_TESTS           ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES        ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON CACHE BOOL "" FORCE)
set(SPIRV_SKIP_TESTS             ON CACHE BOOL "" FORCE)
set(SHADERC_ENABLE_SHARED_CRT    ON CACHE BOOL "" FORCE)
add_subdirectory(${Dependencies}/shaderc shaderc)
target_link_libraries(avocet PUBLIC shaderc)

function(group_external_targets dir folder_name)
    get_property(targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)

    foreach(target ${targets})
        set_property(TARGET ${target} PROPERTY FOLDER ${folder_name})
    endforeach()

    get_property(subdirs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)

    foreach(subdir ${subdirs})
        group_external_targets(${subdir} ${folder_name})
    endforeach()
endfunction()

group_external_targets(${Dependencies}/shaderc Shaderc)

sequoia_finalize_library(avocet)
