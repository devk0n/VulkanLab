cmake_minimum_required(VERSION 3.30)
project(VulkanLab)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find GLFW
find_package(glfw3 REQUIRED)

# Project sources
set(SOURCES
        source/main.cpp
        source/vulkan/VulkanInstance.cpp
        include/core/Application.h
        source/core/Application.cpp
        include/vulkan/VulkanDebugMessenger.h
        source/vulkan/VulkanDebugMessenger.cpp
        include/vulkan/VulkanDevice.h
        source/vulkan/VulkanDevice.cpp
        include/vulkan/VulkanSwapchain.h
        source/vulkan/VulkanSwapchain.cpp
        include/vulkan/VulkanRenderPass.h
        source/vulkan/VulkanRenderPass.cpp
        include/vulkan/VulkanFramebuffer.h
        source/vulkan/VulkanFramebuffer.cpp
)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include paths
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core
        include/vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/source/core
        source/vulkan
)

# Link everything
target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        glfw
)
