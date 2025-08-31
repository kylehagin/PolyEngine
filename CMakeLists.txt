cmake_minimum_required(VERSION 3.10)
project(synty-lite LANGUAGES C CXX)

option(SYNTY_LITE_BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include directories for dependencies
include_directories(
    ${EXTERNAL_DIR}/glfw/include
    ${EXTERNAL_DIR}/volk
    ${EXTERNAL_DIR}/glm
    ${EXTERNAL_DIR}/vma
    ${EXTERNAL_DIR}/assimp/include
    ${EXTERNAL_DIR}/meshoptimizer/src
    ${EXTERNAL_DIR}/stb
    ${CMAKE_SOURCE_DIR}/src
)

add_subdirectory(${EXTERNAL_DIR}/glfw EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_DIR}/volk EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_DIR}/meshoptimizer EXCLUDE_FROM_ALL)

# stb, glm, vma, assimp provide headers only or own projects
find_package(Vulkan REQUIRED)

file(GLOB SRC_FILES
    src/*.cpp
)

add_executable(synty-lite ${SRC_FILES})

target_link_libraries(synty-lite
    PRIVATE
        glfw
        volk
        meshoptimizer
        ${Vulkan_LIBRARIES}
)

target_compile_definitions(synty-lite PRIVATE VK_USE_PLATFORM_WIN32_KHR=1)

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
        set(SPIRV_FILE ${SPIRV_DIR}/${FILE_NAME}.spv)
        add_custom_command(
            OUTPUT ${SPIRV_FILE}
            COMMAND ${GLSLC} -std=450 ${SHADER} -o ${SPIRV_FILE}
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
