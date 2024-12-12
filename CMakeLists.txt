cmake_minimum_required(VERSION 3.30)
project(VulkanLab)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

add_executable(VulkanLab main.cpp
        vl_window.h
        vl_window.cpp
        first_app.h
        first_app.cpp
)

target_link_libraries(VulkanLab glfw)
target_link_libraries(VulkanLab Vulkan::Vulkan)
target_link_libraries(glm::glm)


