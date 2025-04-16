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

# Find GLM
find_package(glm REQUIRED)

# Project sources
set(SOURCES
        source/main.cpp

        source/core/Application.cpp

        source/vulkan/VulkanInstance.cpp
        source/vulkan/VulkanDebugMessenger.cpp
        source/vulkan/VulkanDevice.cpp
        source/vulkan/VulkanSwapchain.cpp
        source/vulkan/VulkanRenderPass.cpp
        source/vulkan/VulkanFramebuffer.cpp
        source/vulkan/VulkanCommandManager.cpp
        source/vulkan/VulkanSyncObjects.cpp
        include/vulkan/VulkanBuffer.h
        source/vulkan/VulkanBuffer.cpp
        include/vulkan/VulkanPipeline.h
        source/vulkan/VulkanPipeline.cpp
)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include paths
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vulkan
        ${CMAKE_CURRENT_SOURCE_DIR}/source
        ${CMAKE_CURRENT_SOURCE_DIR}/source/core
        ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan
)

# Link everything
target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        glfw
        glm
)
