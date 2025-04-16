#ifndef VULKAN_RENDER_PASS_H
#define VULKAN_RENDER_PASS_H

#include <vulkan/vulkan.h>

class VulkanRenderPass {
public:
    VulkanRenderPass(VkDevice device, VkFormat imageFormat);
    ~VulkanRenderPass();

    VulkanRenderPass(const VulkanRenderPass&) = delete;
    VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;

    [[nodiscard]] VkRenderPass get() const { return m_renderPass; }

private:
    VkDevice m_device;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;
};


#endif // VULKAN_RENDER_PASS_H
