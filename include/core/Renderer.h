#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "VulkanConfig.h"
#include "VulkanContext.h"

class VulkanInstance;
class VulkanDebugMessenger;
class WindowManager;
class VulkanDevice;
class VulkanSwapchain;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanCommandManager;
class VulkanSyncObjects;
class VulkanBuffer;
class VulkanPipeline;

class Renderer {
public:
    explicit Renderer(WindowManager& windowManager);
    ~Renderer();

    void draw();
    void onResize();
    void waitIdle() const;

    VulkanContext& getContext();
    VulkanConfig& getConfig();
    [[nodiscard]] uint32_t getGraphicsQueueIndex() const;

private:
    void recreateSwapchain();

    WindowManager& m_windowManager;
    VulkanContext m_context;
    VulkanConfig m_config;

    std::unique_ptr<VulkanInstance> m_instance;
    std::unique_ptr<VulkanDebugMessenger> m_debugMessenger;
    std::unique_ptr<VulkanDevice> m_device;
    std::unique_ptr<VulkanSwapchain> m_swapchain;
    std::unique_ptr<VulkanRenderPass> m_renderPass;
    std::unique_ptr<VulkanFramebuffer> m_framebuffer;
    std::unique_ptr<VulkanCommandManager> m_commandManager;
    std::unique_ptr<VulkanSyncObjects> m_syncObjects;
    std::unique_ptr<VulkanPipeline> m_pipeline;
    std::unique_ptr<VulkanBuffer> m_vertexBuffer;

    size_t m_currentFrame = 0;
    bool m_framebufferResized = false;
};

#endif // RENDERER_H
