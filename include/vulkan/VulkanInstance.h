#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanConfig.h"

class VulkanInstance {
public:
    explicit VulkanInstance(VulkanConfig  config);
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance& operator=(const VulkanInstance&) = delete;
    VulkanInstance(VulkanInstance&&) = delete;
    VulkanInstance& operator=(VulkanInstance&&) = delete;

    [[nodiscard]] VkInstance get() const noexcept { return m_instance; }

private:
    VulkanConfig m_config;
    VkInstance m_instance = VK_NULL_HANDLE;

    [[nodiscard]] std::vector<const char*> getRequiredExtensions() const;
    [[nodiscard]] static bool checkValidationLayerSupport() ;
};

#endif // VULKAN_INSTANCE_H
