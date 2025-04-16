#ifndef VULKAN_COMMAND_MANAGER_H
#define VULKAN_COMMAND_MANAGER_H

#include <vulkan/vulkan.h>
#include <vector>

class VulkanCommandManager {
public:
    VulkanCommandManager(VkDevice device, uint32_t queueFamily, size_t bufferCount);
    ~VulkanCommandManager();

    VulkanCommandManager(const VulkanCommandManager&) = delete;
    VulkanCommandManager& operator=(const VulkanCommandManager&) = delete;

    [[nodiscard]] VkCommandPool getCommandPool() const { return m_commandPool; }
    [[nodiscard]] const std::vector<VkCommandBuffer>& getCommandBuffers() const { return m_commandBuffers; }

private:
    VkDevice m_device;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;
};

#endif // VULKAN_COMMAND_MANAGER_H
