#ifndef VULKAN_PIPELINE_H
#define VULKAN_PIPELINE_H

#include <string>
#include <vulkan/vulkan.h>
#include "Vertex.h"

class VulkanPipeline {
public:
    VulkanPipeline(VkDevice device, VkRenderPass renderPass);
    ~VulkanPipeline();

    VulkanPipeline(const VulkanPipeline&) = delete;
    VulkanPipeline& operator=(const VulkanPipeline&) = delete;

    [[nodiscard]] VkPipeline get() const { return m_pipeline; }
    [[nodiscard]] VkPipelineLayout getLayout() const { return m_pipelineLayout; }

private:
    VkDevice m_device;
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    VkShaderModule loadShaderModule(const std::string& path) const;
};


#endif // VULKAN_PIPELINE_H
