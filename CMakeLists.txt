cmake_minimum_required(VERSION 3.10)
project(VulkanLab)

set(CMAKE_CXX_STANDARD 17)

# Include GLFW and GLM directories
include_directories("C:/Users/devkon/Development/Tools/glfw-3.4.bin.WIN64/include")
include_directories("C:/Users/devkon/Development/Tools/glm/glm")

# Link GLFW library directories
link_directories("C:/Users/devkon/Development/Tools/glfw-3.4.bin.WIN64/lib-mingw-w64")

# Find Vulkan
find_package(Vulkan REQUIRED)

# Add executable
add_executable(VulkanLab src/main.cpp
        10_fixed_functions.
)

# Link libraries (do NOT include GLM here)
target_link_libraries(VulkanLab Vulkan::Vulkan glfw3)
