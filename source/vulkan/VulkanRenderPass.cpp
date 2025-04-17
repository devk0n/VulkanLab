#include "VulkanRenderPass.h"
#include "Logger.h"
#include <stdexcept>

VulkanRenderPass::VulkanRenderPass(VkDevice device, VkFormat imageFormat)
    : m_device(device)
{
    VkAttachmentDescription colorAttachment {
        .format             = imageFormat,
        .samples            = VK_SAMPLE_COUNT_1_BIT,
        .loadOp             = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp            = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp      = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp     = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout      = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout        = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorRef {
        .attachment     = 0,
        .layout         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass {
        .pipelineBindPoint      = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount   = 1,
        .pColorAttachments      = &colorRef
    };

    VkSubpassDependency dependency {
        .srcSubpass     = VK_SUBPASS_EXTERNAL,
        .dstSubpass     = 0,
        .srcStageMask   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask  = 0,
        .dstAccessMask  = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };

    VkRenderPassCreateInfo renderPassInfo {
        .sType              = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount    = 1,
        .pAttachments       = &colorAttachment,
        .subpassCount       = 1,
        .pSubpasses         = &subpass,
        .dependencyCount    = 1,
        .pDependencies      = &dependency
    };

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create render pass.");
    }

    DEBUG("Render pass created.");
}

VulkanRenderPass::~VulkanRenderPass() {
    if (m_renderPass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
        DEBUG("Render pass destroyed.");
    }
}
