cmake_minimum_required(VERSION 3.10)
project(synty-lite LANGUAGES C CXX)

option(SYNTY_LITE_BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Configure third-party builds
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
if(NOT WIN32)
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
endif()

# Include directories for dependencies
set(SYNTY_LITE_INCLUDE_DIRS
    ${EXTERNAL_DIR}/glfw/include
    ${EXTERNAL_DIR}/volk
    ${EXTERNAL_DIR}/glm
    ${EXTERNAL_DIR}/vma/include
    ${EXTERNAL_DIR}/assimp/include
    ${EXTERNAL_DIR}/meshoptimizer/src
    ${EXTERNAL_DIR}/stb
    ${CMAKE_SOURCE_DIR}/src
)

if(MSVC)
    # Ensure the CRT exposes C99 float math symbols like powf/remainderf that
    # meshoptimizer and other translation units include via <cmath> when
    # building with MSVC. Setting this at directory scope applies to all
    # targets defined below (including meshoptimizer and synty-lite) so they
    # see the same declarations.
    add_compile_definitions(_CRT_DECLARE_NONSTDC_NAMES=1)
endif()

add_subdirectory(${EXTERNAL_DIR}/glfw EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_DIR}/volk EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_DIR}/meshoptimizer EXCLUDE_FROM_ALL)

# stb, glm, vma, assimp provide headers only or own projects
find_package(Vulkan REQUIRED)

file(GLOB SRC_FILES
    src/*.cpp
)

add_executable(synty-lite ${SRC_FILES})

target_include_directories(synty-lite PRIVATE ${SYNTY_LITE_INCLUDE_DIRS})

target_link_libraries(synty-lite
    PRIVATE
        glfw
        volk
        meshoptimizer
        ${Vulkan_LIBRARIES}
)

if(WIN32)
    target_compile_definitions(synty-lite PRIVATE VK_USE_PLATFORM_WIN32_KHR)
elseif(APPLE)
    target_compile_definitions(synty-lite PRIVATE VK_USE_PLATFORM_MACOS_MVK)
else()
    target_compile_definitions(synty-lite PRIVATE VK_USE_PLATFORM_XCB_KHR)
endif()

target_compile_definitions(synty-lite PRIVATE VOLK_STATIC_DEFINE)

# Shader compilation
set(SHADER_DIR ${CMAKE_SOURCE_DIR}/shaders)
set(SPIRV_DIR ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${SPIRV_DIR})

find_program(GLSLC glslc)
if(GLSLC)
    message(STATUS "glslc found: ${GLSLC}")
    file(GLOB SHADERS ${SHADER_DIR}/*.glsl)
    foreach(SHADER ${SHADERS})
        get_filename_component(FILE_NAME ${SHADER} NAME)

        # Extract shader stage from filename (e.g. cull.comp.glsl -> comp)
        string(REGEX REPLACE "^[^.]+\\.([^.]+)\\.glsl$" "\\1" STAGE ${FILE_NAME})

        if(STAGE STREQUAL "vert")
            set(GLSL_STAGE vertex)
        elseif(STAGE STREQUAL "frag")
            set(GLSL_STAGE fragment)
        elseif(STAGE STREQUAL "comp")
            set(GLSL_STAGE compute)
        else()
            message(FATAL_ERROR "Unknown shader stage: ${STAGE} for ${FILE_NAME}")
        endif()

        set(SPIRV_FILE ${SPIRV_DIR}/${FILE_NAME}.spv)
        add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLC} -std=450 -fshader-stage=${GLSL_STAGE} ${SHADER} -o ${SPIRV_FILE}
            DEPENDS ${SHADER}
            COMMENT "Compiling ${FILE_NAME}"
        )
        list(APPEND SPIRV_BINS ${SPIRV_FILE})
    endforeach()
    add_custom_target(shaders ALL DEPENDS ${SPIRV_BINS})
    add_dependencies(synty-lite shaders)
else()
    message(WARNING "glslc not found – shaders compiled at runtime")
endif()

if(SYNTY_LITE_BUILD_TESTS)
    enable_testing()
    # Placeholder for future tests
endif()
