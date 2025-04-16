#ifndef VULKAN_DEBUG_MESSENGER_H
#define VULKAN_DEBUG_MESSENGER_H

#include <vulkan/vulkan.h>

class VulkanDebugMessenger {
public:
    explicit VulkanDebugMessenger(VkInstance instance);
    ~VulkanDebugMessenger();

    VulkanDebugMessenger(const VulkanDebugMessenger&) = delete;
    VulkanDebugMessenger& operator=(const VulkanDebugMessenger&) = delete;

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_messenger = VK_NULL_HANDLE;

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);
};

#endif // VULKAN_DEBUG_MESSENGER_H
