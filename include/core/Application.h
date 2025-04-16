#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>
#include <GLFW/glfw3.h>

class VulkanInstance;
class VulkanDebugMessenger;
class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanFramebuffer;

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    void initialize();
    void mainLoop();
    void cleanup();

    GLFWwindow* m_window = nullptr;
    std::unique_ptr<VulkanInstance> m_vulkanInstance;
    std::unique_ptr<VulkanDebugMessenger> m_debugMessenger;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanSwapchain> m_swapchain;
    std::unique_ptr<VulkanRenderPass> m_renderPass;
    std::unique_ptr<VulkanFramebuffer> m_framebuffer;
};

#endif // APPLICATION_H
