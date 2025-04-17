#include "VulkanPipeline.h"
#include <vector>
#include <fstream>
#include <stdexcept>

VulkanPipeline::VulkanPipeline(VkDevice device, VkRenderPass renderPass)
    : m_device(device)
{
    VkShaderModule vertShader = loadShaderModule("/home/devkon/CLionProjects/VulkanLab/assets/shaders/triangle.vert.spv");
    VkShaderModule fragShader = loadShaderModule("/home/devkon/CLionProjects/VulkanLab/assets/shaders/triangle.frag.spv");

    VkPipelineShaderStageCreateInfo vertStage {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShader,
        .pName  = "main"
    };

    VkPipelineShaderStageCreateInfo fragStage {
        .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShader,
        .pName  = "main"
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertStage, fragStage };

    // Vertex input
    const auto bindingDescriptions = Vertex::getBindingDescription();
    const auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInput {
        .sType                              = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount      = 1,
        .pVertexBindingDescriptions         = &bindingDescriptions,
        .vertexAttributeDescriptionCount    = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions       = attributeDescriptions.data()
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {
        .sType                      = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology                   = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable     = VK_FALSE
    };

    VkPipelineViewportStateCreateInfo viewportState {
        .sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount  = 1,
        .pViewports     = nullptr,
        .scissorCount   = 1,
        .pScissors      = nullptr
    };

    VkPipelineRasterizationStateCreateInfo rasterizer {
        .sType          = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode    = VK_POLYGON_MODE_FILL,
        .cullMode       = VK_CULL_MODE_BACK_BIT,
        .frontFace      = VK_FRONT_FACE_CLOCKWISE,
        .lineWidth      = 1.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampling {
        .sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                          VK_COLOR_COMPONENT_G_BIT |
                          VK_COLOR_COMPONENT_B_BIT |
                          VK_COLOR_COMPONENT_A_BIT
    };

    VkPipelineColorBlendStateCreateInfo colorBlending {
        .sType              = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount    = 1,
        .pAttachments       = &colorBlendAttachment
    };

    // Layout
    VkPipelineLayoutCreateInfo layoutInfo {
        .sType              = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO
    };

    if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        throw std::runtime_error("Failed to create pipeline layout.");

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState {
        .sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount  = 2,
        .pDynamicStates     = dynamicStates
    };

    // Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo {
        .sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount             = 2,
        .pStages                = shaderStages,
        .pVertexInputState      = &vertexInput,
        .pInputAssemblyState    = &inputAssembly,
        .pViewportState         = &viewportState,
        .pRasterizationState    = &rasterizer,
        .pMultisampleState      = &multisampling,
        .pColorBlendState       = &colorBlending,
        .pDynamicState          = &dynamicState,
        .layout                 = m_pipelineLayout,
        .renderPass             = renderPass,
        .subpass                = 0
    };

    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS)
        throw std::runtime_error("Failed to create graphics pipeline.");

    vkDestroyShaderModule(device, vertShader, nullptr);
    vkDestroyShaderModule(device, fragShader, nullptr);
}

VulkanPipeline::~VulkanPipeline() {
    if (m_pipeline) vkDestroyPipeline(m_device, m_pipeline, nullptr);
    if (m_pipelineLayout) vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
}

VkShaderModule VulkanPipeline::loadShaderModule(const std::string& path) const {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Failed to open shader file: " + path);

    size_t size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0);
    file.read(buffer.data(), size);

    VkShaderModuleCreateInfo createInfo {
        .sType      = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize   = buffer.size(),
        .pCode      = reinterpret_cast<const uint32_t*>(buffer.data())
    };

    VkShaderModule shader;
    if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shader) != VK_SUCCESS)
        throw std::runtime_error("Failed to create shader module.");

    return shader;
}
