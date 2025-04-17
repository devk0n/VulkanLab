#include "VulkanFramebuffer.h"
#include "Logger.h"
#include <stdexcept>

VulkanFramebuffer::VulkanFramebuffer(
    VkDevice device,
    VkRenderPass renderPass,
    const std::vector<VkImageView>& imageViews,
    VkExtent2D extent
) : m_device(device)
{
    m_framebuffers.reserve(imageViews.size());

    for (const auto& view : imageViews) {
        VkImageView attachments[] = { view };

        VkFramebufferCreateInfo info {
            .sType              = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass         = renderPass,
            .attachmentCount    = 1,
            .pAttachments       = attachments,
            .width              = extent.width,
            .height             = extent.height,
            .layers             = 1
        };

        VkFramebuffer fb;
        if (vkCreateFramebuffer(device, &info, nullptr, &fb) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create framebuffer.");
        }

        m_framebuffers.push_back(fb);
    }

    DEBUG("Created ", m_framebuffers.size(), " framebuffers.");
}

VulkanFramebuffer::~VulkanFramebuffer() {
    for (auto fb : m_framebuffers) {
        vkDestroyFramebuffer(m_device, fb, nullptr);
    }
    DEBUG("Framebuffers destroyed.");
}
