#include "VulkanDevice.h"

#include <set>

#include "Logger.h"

#include <vector>
#include <stdexcept>

#include "WindowManager.h"

const std::vector deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VulkanDevice::VulkanDevice(VkInstance instance, const WindowManager& windowManager)
    : m_instance(instance) {

    createSurface(windowManager.get());
    pickPhysicalDevice();
    createLogicalDevice();

}

VulkanDevice::~VulkanDevice() {
    if (m_surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        DEBUG("Surface destroyed.");
    }

    if (m_device != VK_NULL_HANDLE) {
        vkDestroyDevice(m_device, nullptr);
        DEBUG("Logical device destroyed.");
    }

}

void VulkanDevice::createSurface(GLFWwindow* window) {
    if (glfwCreateWindowSurface(m_instance, window, nullptr, &m_surface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface.");
    }
}

void VulkanDevice::pickPhysicalDevice() {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(m_instance, &count, nullptr);
    if (count == 0) throw std::runtime_error("No Vulkan-supported GPUs found.");

    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(m_instance, &count, devices.data());

    for (const auto& device : devices) {
        const QueueFamilyIndices indices = findQueueFamilies(device);
        if (indices.isComplete()) {
            m_physicalDevice = device;
            m_queueIndices = indices;
            DEBUG("Physical device selected.");
            return;
        }
    }

    throw std::runtime_error("No suitable GPU found.");
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) const {
    QueueFamilyIndices indices;

    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
    std::vector<VkQueueFamilyProperties> families(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

    for (uint32_t i = 0; i < count; ++i) {
        const auto& props = families[i];

        if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphics = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
        if (presentSupport) {
            indices.present = i;
        }

        if (indices.isComplete()) break;
    }

    return indices;
}

void VulkanDevice::createLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set uniqueQueueFamilies = {
        m_queueIndices.graphics.value(),
        m_queueIndices.present.value()
    };

    float queuePriority = 1.0f;
    for (const uint32_t family : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueInfo {
            .sType              = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex   = family,
            .queueCount         = 1,
            .pQueuePriorities   = &queuePriority
        };
        queueCreateInfos.push_back(queueInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{}; // Fill later when needed

    VkDeviceCreateInfo createInfo {
        .sType                      = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount       = static_cast<uint32_t>(queueCreateInfos.size()),
        .pQueueCreateInfos          = queueCreateInfos.data(),
        .enabledExtensionCount      = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames    = deviceExtensions.data(),
        .pEnabledFeatures           = &deviceFeatures
    };

    const auto result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    if (result != VK_SUCCESS || m_device == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to create logical device.");
    }

    DEBUG("Logical device created.");

    vkGetDeviceQueue(m_device, m_queueIndices.graphics.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_queueIndices.present.value(), 0, &m_presentQueue);
}

