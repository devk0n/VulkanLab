#include "ImGuiLayer.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <stdexcept>

ImGuiLayer::ImGuiLayer(
    GLFWwindow* window,
    const VulkanContext& context,
    const VulkanConfig& config)
    : m_device(context.device) {
    // Descriptor pool for ImGui
    createDescriptorPool();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window, true);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance           = context.instance;
    initInfo.PhysicalDevice     = context.physicalDevice;
    initInfo.Device             = context.device;
    initInfo.QueueFamily        = context.graphicsQueueFamily;
    initInfo.Queue              = context.graphicsQueue;
    initInfo.DescriptorPool     = m_descriptorPool;
    initInfo.MinImageCount      = config.maxFramesInFlight;
    initInfo.ImageCount         = config.maxFramesInFlight;
    initInfo.RenderPass         = context.renderPass;

    ImGui_ImplVulkan_Init(&initInfo);
}

ImGuiLayer::~ImGuiLayer() {
    vkDeviceWaitIdle(m_device);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);
}

void ImGuiLayer::createDescriptorPool() {
    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    };

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags          = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets        = 1000;
    poolInfo.poolSizeCount  = 1;
    poolInfo.pPoolSizes     = poolSizes;

    if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create ImGui descriptor pool.");
    }
}

void ImGuiLayer::beginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::endFrame(VkCommandBuffer cmd) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void ImGuiLayer::rebuildFontAtlas(VkCommandBuffer cmd) {
    ImGui_ImplVulkan_CreateFontsTexture();
}
