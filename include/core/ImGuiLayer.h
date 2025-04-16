#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

class ImGuiLayer {
public:
    ImGuiLayer(GLFWwindow* window, VkInstance instance, VkDevice device,
               VkPhysicalDevice physicalDevice, VkQueue graphicsQueue,
               uint32_t graphicsQueueFamily, VkRenderPass renderPass,
               uint32_t imageCount);

    ~ImGuiLayer();

    void beginFrame();
    void endFrame(VkCommandBuffer cmd);

    void rebuildFontAtlas(VkCommandBuffer cmd);

private:
    VkDevice m_device;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;

    void createDescriptorPool();
};

#endif // IMGUI_LAYER_H
