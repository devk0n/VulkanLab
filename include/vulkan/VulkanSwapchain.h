#ifndef VULKAN_SWAPCHAIN_H
#define VULKAN_SWAPCHAIN_H

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdint>

class VulkanSwapchain {
public:
    VulkanSwapchain(
        VkPhysicalDevice physicalDevice,
        VkDevice device,
        VkSurfaceKHR surface,
        uint32_t graphicsQueueFamily,
        uint32_t presentQueueFamily,
        uint32_t windowWidth,
        uint32_t windowHeight,
        VkSwapchainKHR oldSwapchain = VK_NULL_HANDLE
    );

    ~VulkanSwapchain();

    VulkanSwapchain(const VulkanSwapchain&) = delete;
    VulkanSwapchain& operator=(const VulkanSwapchain&) = delete;

    [[nodiscard]] VkSwapchainKHR get() const { return m_swapchain; }
    [[nodiscard]] const std::vector<VkImageView>& getImageViews() const { return m_imageViews; }
    [[nodiscard]] VkFormat getImageFormat() const { return m_imageFormat; }
    [[nodiscard]] VkExtent2D getExtent() const { return m_extent; }

private:
    VkDevice m_device;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;
    VkFormat m_imageFormat;
    VkExtent2D m_extent;

    VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& modes);
    VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

    void createImageViews();
};

#endif // VULKAN_SWAPCHAIN_H
