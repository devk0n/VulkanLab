#include "VulkanSyncObjects.h"
#include "Logger.h"
#include <stdexcept>

VulkanSyncObjects::VulkanSyncObjects(VkDevice device, size_t frameCount)
    : m_device(device)
{
    m_frames.resize(frameCount);

    VkSemaphoreCreateInfo semaphoreInfo {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    for (size_t i = 0; i < frameCount; ++i) {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_frames[i].imageAvailable) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_frames[i].renderFinished) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &m_frames[i].inFlight) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create sync objects for frame.");
            }
    }

    DEBUG("Created sync objects for ", frameCount, " frames.");
}

VulkanSyncObjects::~VulkanSyncObjects() {
    for (const auto& frame : m_frames) {
        vkDestroyFence(m_device, frame.inFlight, nullptr);
        vkDestroySemaphore(m_device, frame.renderFinished, nullptr);
        vkDestroySemaphore(m_device, frame.imageAvailable, nullptr);
    }
    DEBUG("Destroyed sync objects.");
}
