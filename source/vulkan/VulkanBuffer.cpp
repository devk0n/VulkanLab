#include "VulkanBuffer.h"
#include <stdexcept>

VulkanBuffer::VulkanBuffer(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties)
        : m_device(device) {

    VkBufferCreateInfo bufferInfo {
        .sType          = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size           = size,
        .usage          = usage,
        .sharingMode    = VK_SHARING_MODE_EXCLUSIVE
    };

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create buffer.");
    }

    allocate(physicalDevice, properties, size);
}

void VulkanBuffer::allocate(VkPhysicalDevice physicalDevice, VkMemoryPropertyFlags properties, VkDeviceSize size) {
    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(m_device, m_buffer, &memReqs);

    VkMemoryAllocateInfo allocInfo {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memReqs.size,
        .memoryTypeIndex = findMemoryType(physicalDevice, memReqs.memoryTypeBits, properties)
    };

    if (vkAllocateMemory(m_device, &allocInfo, nullptr, &m_memory) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate buffer memory.");
    }

    vkBindBufferMemory(m_device, m_buffer, m_memory, 0);
}

uint32_t VulkanBuffer::findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        if (typeFilter & 1 << i &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type.");
}

VulkanBuffer::~VulkanBuffer() {
    if (m_buffer) vkDestroyBuffer(m_device, m_buffer, nullptr);
    if (m_memory) vkFreeMemory(m_device, m_memory, nullptr);
}
