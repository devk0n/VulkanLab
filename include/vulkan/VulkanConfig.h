#ifndef VULKAN_CONFIG_H
#define VULKAN_CONFIG_H

#include <vulkan/vulkan.h>
#include <vector>

struct VulkanConfig {
    // Debug/Validation
    bool enableValidationLayers = true;
    std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

    // Device Extensions
    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    // Swapchain Preferences
    VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; // Prefer no vsync
    VkFormat preferredSurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB;             // sRGB color space

    // Pipeline Defaults
    VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL; // Wireframe vs. solid
    VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT; // Back-face culling

    // Application-specific settings
    uint32_t maxFramesInFlight = 2;

    void setWireframeMode(bool enabled) {
        polygonMode = enabled ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    }
};

#endif // VULKAN_CONFIG_H
