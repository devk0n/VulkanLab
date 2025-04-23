#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include <vulkan/vulkan.h>

struct VulkanContext {
    VkInstance instance;
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkRenderPass renderPass;
    VkExtent2D swapchainExtent;
    VkFormat swapchainImageFormat;
    uint32_t graphicsQueueFamily;
};

#endif // VULKAN_CONTEXT_H
