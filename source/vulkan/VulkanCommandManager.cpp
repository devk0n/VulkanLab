#include "VulkanCommandManager.h"
#include "Logger.h"
#include <stdexcept>

VulkanCommandManager::VulkanCommandManager(VkDevice device, uint32_t queueFamily, size_t bufferCount)
    : m_device(device)
{
    VkCommandPoolCreateInfo poolInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = queueFamily
    };

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool.");
    }

    INFO("Command pool created.");

    m_commandBuffers.resize(bufferCount);

    VkCommandBufferAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = m_commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32_t>(bufferCount)
    };

    if (vkAllocateCommandBuffers(device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffers.");
    }

    INFO("Allocated ", bufferCount, " command buffers.");
}

VulkanCommandManager::~VulkanCommandManager() {
    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
        INFO("Command pool destroyed.");
    }
}
