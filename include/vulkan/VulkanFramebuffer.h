#ifndef VULKAN_FRAMEBUFFER_H
#define VULKAN_FRAMEBUFFER_H

#include <vulkan/vulkan.h>
#include <vector>

class VulkanFramebuffer {
public:
    VulkanFramebuffer(VkDevice device, VkRenderPass renderPass,
                      const std::vector<VkImageView>& imageViews,
                      VkExtent2D extent);

    ~VulkanFramebuffer();

    VulkanFramebuffer(const VulkanFramebuffer&) = delete;
    VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

    [[nodiscard]] const std::vector<VkFramebuffer>& getFramebuffers() const { return m_framebuffers; }

private:
    VkDevice m_device;
    std::vector<VkFramebuffer> m_framebuffers;
};

#endif // VULKAN_FRAMEBUFFER_H
