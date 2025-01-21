cmake_minimum_required(VERSION 3.10)

# Set project name and version
project(VulkanLab VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define paths to external libraries
set(GLFW_ROOT "C:/Users/devkon/Development/Tools/glfw-3.4.bin.WIN64")
set(GLM_ROOT "C:/Users/devkon/Development/Tools/glm")

# Include directories for GLFW and GLM
include_directories(
    ${GLFW_ROOT}/include
    ${GLM_ROOT}
)

# Link GLFW library directories
link_directories(${GLFW_ROOT}/lib-mingw-w64)

# Find Vulkan package
find_package(Vulkan REQUIRED)

# Add source files
set(SOURCE_FILES
    src/main.cpp
)

# Create the executable target
add_executable(VulkanLab ${SOURCE_FILES})

# Link required libraries (do NOT link GLM since it's a header-only library)
target_link_libraries(VulkanLab PRIVATE Vulkan::Vulkan glfw3)

# Define preprocessor definitions based on build type
target_compile_definitions(VulkanLab PRIVATE
    $<$<CONFIG:Release>:NDEBUG>
)

# Print configuration summary
message(STATUS "Project: VulkanLab")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "GLFW Root: ${GLFW_ROOT}")
message(STATUS "GLM Root: ${GLM_ROOT}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
