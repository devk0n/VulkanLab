#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "VulkanContext.h"
#include "VulkanConfig.h"

class ImGuiLayer {
public:
    explicit ImGuiLayer(
        GLFWwindow* window,
        const VulkanContext& context,
        const VulkanConfig& config);

    ~ImGuiLayer();

    static void beginFrame();
    static void endFrame(VkCommandBuffer cmd);

    static void rebuildFontAtlas(VkCommandBuffer cmd);

private:
    void createDescriptorPool();

    VkDevice m_device;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;

};

#endif // IMGUI_LAYER_H
