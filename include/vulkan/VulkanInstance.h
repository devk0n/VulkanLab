#ifndef VULKAN_INSTANCE_H
#define VULKAN_INSTANCE_H

#include <vulkan/vulkan.h>
#include <vector>

class VulkanInstance {
public:
    explicit(true) VulkanInstance(bool enableValidation);
    ~VulkanInstance();

    VulkanInstance(const VulkanInstance&) = delete;
    VulkanInstance& operator=(const VulkanInstance&) = delete;
    VulkanInstance(VulkanInstance&&) = delete;
    VulkanInstance& operator=(VulkanInstance&&) = delete;

    [[nodiscard]] VkInstance get() const noexcept { return m_instance; }

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    bool m_enableValidation = false;

    [[nodiscard]] std::vector<const char*> getRequiredExtensions() const;
    [[nodiscard]] bool checkValidationLayerSupport() const;
};

#endif // VULKAN_INSTANCE_H
