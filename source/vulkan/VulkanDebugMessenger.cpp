#include "VulkanDebugMessenger.h"
#include "Logger.h"

#include <stdexcept>
#include <format>

VulkanDebugMessenger::VulkanDebugMessenger(VkInstance instance)
    : m_instance(instance)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = &VulkanDebugMessenger::debugCallback
    };

    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));

    if (!func || func(m_instance, &createInfo, nullptr, &m_messenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up Vulkan debug messenger.");
    }

    INFO("Vulkan debug messenger initialized.");
}

VulkanDebugMessenger::~VulkanDebugMessenger() {
    if (m_messenger) {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
        func(m_instance, m_messenger, nullptr);
        INFO("Vulkan debug messenger destroyed.");
    }
}

VkBool32 VKAPI_CALL VulkanDebugMessenger::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void*)
{
    const std::string_view message = callbackData->pMessage;

    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        ERROR("Vulkan: ", message);
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        WARN("Vulkan: ", message);
    } else {
        DEBUG("Vulkan: ", message);
    }

    return VK_FALSE;
}
