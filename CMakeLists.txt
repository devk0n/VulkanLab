cmake_minimum_required(VERSION 3.30)
project(VulkanLab)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable FetchContent
include(FetchContent)

# Download ImGui
FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG        v1.91.9b
)
FetchContent_MakeAvailable(imgui)
add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_SOURCE_DIR}
)

# Find dependencies
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Project sources
set(SOURCES
        source/main.cpp

        source/core/Application.cpp
        source/core/WindowManager.cpp
        source/core/ImGuiLayer.cpp

        source/vulkan/VulkanInstance.cpp
        source/vulkan/VulkanDebugMessenger.cpp
        source/vulkan/VulkanDevice.cpp
        source/vulkan/VulkanSwapchain.cpp
        source/vulkan/VulkanRenderPass.cpp
        source/vulkan/VulkanFramebuffer.cpp
        source/vulkan/VulkanCommandManager.cpp
        source/vulkan/VulkanSyncObjects.cpp
        source/vulkan/VulkanBuffer.cpp
        source/vulkan/VulkanPipeline.cpp

        # ImGui backends
        ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        include/vulkan/VulkanConfig.h
        include/core/Renderer.h
        include/vulkan/VulkanContext.h
        source/core/Renderer.cpp
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

        # ImGui
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        glfw
        glm
        imgui
)