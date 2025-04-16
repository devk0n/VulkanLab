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
class VulkanCommandManager;
class VulkanSyncObjects;
class VulkanBuffer;

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    void initialize();
    void mainLoop();

    void drawFrame();

    void cleanup();

    GLFWwindow* m_window = nullptr;
    std::unique_ptr<VulkanInstance> m_vulkanInstance;
    std::unique_ptr<VulkanDebugMessenger> m_debugMessenger;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanSwapchain> m_swapchain;
    std::unique_ptr<VulkanRenderPass> m_renderPass;
    std::unique_ptr<VulkanFramebuffer> m_framebuffer;
    std::unique_ptr<VulkanCommandManager> m_commandManager;
    static constexpr size_t MAX_FRAMES_IN_FLIGHT = 2;
    size_t m_currentFrame = 0;
    std::unique_ptr<VulkanSyncObjects> m_syncObjects;
    std::unique_ptr<VulkanBuffer> m_vertexBuffer;
};

#endif // APPLICATION_H
