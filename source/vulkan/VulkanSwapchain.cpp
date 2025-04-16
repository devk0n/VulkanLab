#include "VulkanSwapchain.h"
#include "Logger.h"

#include <stdexcept>
#include <algorithm>

VulkanSwapchain::VulkanSwapchain(
    VkPhysicalDevice physicalDevice,
    VkDevice device,
    VkSurfaceKHR surface,
    uint32_t graphicsQueueFamily,
    uint32_t presentQueueFamily,
    uint32_t windowWidth,
    uint32_t windowHeight,
    VkSwapchainKHR oldSwapchain
) : m_device(device) {

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

    uint32_t modeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(modeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &modeCount, presentModes.data());

    const VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(formats);
    const VkPresentModeKHR presentMode = choosePresentMode(presentModes);
    m_extent = chooseExtent(capabilities, windowWidth, windowHeight);
    m_imageFormat = surfaceFormat.format;

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
        imageCount = capabilities.maxImageCount;
    }
    uint32_t queueFamilies[] = { graphicsQueueFamily, presentQueueFamily };

    VkSwapchainCreateInfoKHR createInfo {
        .sType                  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext                  = nullptr,
        .flags                  = 0,
        .surface                = surface,
        .minImageCount          = imageCount,
        .imageFormat            = surfaceFormat.format,
        .imageColorSpace        = surfaceFormat.colorSpace,
        .imageExtent            = m_extent,
        .imageArrayLayers       = 1,
        .imageUsage             = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode       = (graphicsQueueFamily != presentQueueFamily
                                    ? VK_SHARING_MODE_CONCURRENT
                                    : VK_SHARING_MODE_EXCLUSIVE),
        .queueFamilyIndexCount  = (graphicsQueueFamily != presentQueueFamily ? 2u : 0u),
        .pQueueFamilyIndices    = (graphicsQueueFamily != presentQueueFamily
                                    ? queueFamilies
                                    : nullptr),
        .preTransform           = capabilities.currentTransform,
        .compositeAlpha         = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode            = presentMode,
        .clipped                = VK_TRUE,
        .oldSwapchain           = oldSwapchain,
    };

    if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create swapchain.");
    }

    vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, nullptr);
    m_images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, m_images.data());

    createImageViews();
    DEBUG("Swapchain created with ", m_images.size(),  " images.");
}

VulkanSwapchain::~VulkanSwapchain() {
    for (auto view : m_imageViews) {
        vkDestroyImageView(m_device, view, nullptr);
    }
    if (m_swapchain) {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    }
    DEBUG("Swapchain destroyed.");
}

VkSurfaceFormatKHR VulkanSwapchain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }
    return formats[0];
}

VkPresentModeKHR VulkanSwapchain::choosePresentMode(const std::vector<VkPresentModeKHR>& modes) {
    // IMMEDIATE avoids driver-side vsync or mailbox buffering
    for (auto mode : modes) {
        if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            return mode;
    }
    return VK_PRESENT_MODE_FIFO_KHR; // fallback
}


VkExtent2D VulkanSwapchain::chooseExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
    if (capabilities.currentExtent.width != UINT32_MAX) return capabilities.currentExtent;
    return {
        std::clamp(width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width),
        std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

void VulkanSwapchain::createImageViews() {
    m_imageViews.reserve(m_images.size());

    for (const auto& image : m_images) {
        VkImageViewCreateInfo viewInfo {
            .sType              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image              = image,
            .viewType           = VK_IMAGE_VIEW_TYPE_2D,
            .format             = m_imageFormat,
            .components         = {},
            .subresourceRange   = {
                .aspectMask         = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel       = 0,
                .levelCount         = 1,
                .baseArrayLayer     = 0,
                .layerCount         = 1,
            }
        };

        VkImageView imageView;
        if (vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create image view.");
        }
        m_imageViews.push_back(imageView);
    }
}
