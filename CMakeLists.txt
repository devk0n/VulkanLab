cmake_minimum_required(VERSION 3.30)
project(VulkanLab)

set(CMAKE_CXX_STANDARD 17)

# Include GLFW and GLM headers manually
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add executable
add_executable(VulkanLab src/main.cpp)

# Link GLFW manually
target_include_directories(VulkanLab PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(VulkanLab ${CMAKE_SOURCE_DIR}/include/GLFW)

# Find Vulkan
find_package(Vulkan REQUIRED)
target_link_libraries(VulkanLab Vulkan::Vulkan)

# GLM is a header-only library, just include the headers
include_directories(${CMAKE_SOURCE_DIR}/include/glm)
