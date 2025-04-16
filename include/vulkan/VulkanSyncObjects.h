#ifndef VULKAN_SYNC_OBJECTS_H
#define VULKAN_SYNC_OBJECTS_H

#include <vulkan/vulkan.h>
#include <vector>

class VulkanSyncObjects {
public:
    VulkanSyncObjects(VkDevice device, size_t frameCount);
    ~VulkanSyncObjects();

    VulkanSyncObjects(const VulkanSyncObjects&) = delete;
    VulkanSyncObjects& operator=(const VulkanSyncObjects&) = delete;

    struct FrameSync {
        VkSemaphore imageAvailable;
        VkSemaphore renderFinished;
        VkFence inFlight;
    };

    [[nodiscard]] const FrameSync& getFrameSync(size_t frameIndex) const { return m_frames[frameIndex]; }

private:
    VkDevice m_device;
    std::vector<FrameSync> m_frames;
};

#endif // VULKAN_SYNC_OBJECTS_H
