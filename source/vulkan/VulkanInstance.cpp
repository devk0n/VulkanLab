#include "VulkanInstance.h"

#include <cstring>
#include <expected>
#include <stdexcept>
#include <iostream>
#include <bits/ranges_algo.h>
#include <GLFW/glfw3.h>
#include "Logger.h"

namespace {
const std::array validation_layers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::array<const char*, validation_layers.size()>& validationLayers() {
    return validation_layers;
}
}

VulkanInstance::VulkanInstance(bool enableValidation)
    : m_enableValidation(enableValidation)
{
    if (m_enableValidation && !checkValidationLayerSupport()) {
        throw std::runtime_error("Validation layers requested but not available.");
    }

    const VkApplicationInfo appInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Fracture Framework",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Fracture",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    const auto extensions = getRequiredExtensions();

    const VkInstanceCreateInfo createInfo {
        .sType                      = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext                      = VK_NULL_HANDLE,
        .flags                      = 0,
        .pApplicationInfo           = &appInfo,
        .enabledLayerCount          = m_enableValidation ? static_cast<uint32_t>(validationLayers().size()) : 0,
        .ppEnabledLayerNames        = m_enableValidation ? validationLayers().data() : VK_NULL_HANDLE,
        .enabledExtensionCount      = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames    = extensions.data(),
    };

    if (const auto result = vkCreateInstance(&createInfo, nullptr, &m_instance);
        result != VK_SUCCESS || m_instance == VK_NULL_HANDLE) {
        throw std::runtime_error(std::format("Failed to create Vulkan instance. VkResult: {}", static_cast<int>(result)));
        }

    DEBUG("Vulkan instance created successfully.");
}

VulkanInstance::~VulkanInstance() {
    if (m_instance != VK_NULL_HANDLE) {
        vkDestroyInstance(m_instance, nullptr);
        DEBUG("Vulkan instance destroyed.");
    }
}

std::vector<const char*> VulkanInstance::getRequiredExtensions() const {
    uint32_t count = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    std::vector extensions(glfwExtensions, glfwExtensions + count);

    if (m_enableValidation) {
        extensions.push_back("VK_EXT_debug_utils");
    }

    return extensions;
}

bool VulkanInstance::checkValidationLayerSupport() const {
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> available(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, available.data());

    for (const auto& requested : validationLayers()) {
        const bool found = std::ranges::any_of(available, [&](const auto& layer) {
            return std::strcmp(layer.layerName, requested) == 0;
        });

        if (!found) {
            WARN("Missing validation layer: {}", requested);
            return false;
        }
    }
    return true;
}

