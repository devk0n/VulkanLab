#include "Renderer.h"

#include <imgui.h>
#include <cstring>

#include "Logger.h"
#include "WindowManager.h"
#include "ImGuiLayer.h"
#include "VulkanCommandManager.h"
#include "VulkanInstance.h"
#include "VulkanDebugMessenger.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanSyncObjects.h"
#include "Vertex.h"
#include "VulkanBuffer.h"
#include "VulkanPipeline.h"

static std::vector<Vertex> vertices = {
    { {  0.0f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
    { {  0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
    { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } }
};

Renderer::Renderer(WindowManager& windowManager)
    : m_windowManager(windowManager) {

    m_config.enableValidationLayers = true;
    m_config.preferredPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    m_instance = std::make_unique<VulkanInstance>(m_config);

    if (m_config.enableValidationLayers) {
        m_debugMessenger = std::make_unique<VulkanDebugMessenger>(m_instance->get());
    }

    m_device = std::make_unique<VulkanDevice>(m_instance->get(), m_windowManager);
    auto extent = m_windowManager.getExtent();

    const auto&[graphics, present] = m_device->getQueueIndices();
        m_swapchain = std::make_unique<VulkanSwapchain>(
        m_device->getPhysicalDevice(),
        m_device->getDevice(),
        m_config,
        m_device->getSurface(),
        graphics.value(),
        present.value(),
        extent.width, extent.height
    );

    m_renderPass = std::make_unique<VulkanRenderPass>(
        m_device->getDevice(),
        m_swapchain->getImageFormat()
    );

    m_framebuffer = std::make_unique<VulkanFramebuffer>(
        m_device->getDevice(),
        m_renderPass->get(),
        m_swapchain->getImageViews(),
        extent
    );

    m_commandManager = std::make_unique<VulkanCommandManager>(
        m_device->getDevice(),
        m_device->getQueueIndices().graphics.value(),
        m_config.maxFramesInFlight
    );

    m_syncObjects = std::make_unique<VulkanSyncObjects>(
        m_device->getDevice(),
        m_config.maxFramesInFlight
    );

    m_pipeline = std::make_unique<VulkanPipeline>(
        m_device->getDevice(),
        m_renderPass->get()
    );

    m_vertexBuffer = std::make_unique<VulkanBuffer>(
        m_device->getDevice(),
        m_device->getPhysicalDevice(),
        sizeof(Vertex) * vertices.size(),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    // Upload vertex data
    void* data;
    vkMapMemory(m_device->getDevice(), m_vertexBuffer->getMemory(), 0, VK_WHOLE_SIZE, 0, &data);
    memcpy(data, vertices.data(), sizeof(Vertex) * vertices.size());
    vkUnmapMemory(m_device->getDevice(), m_vertexBuffer->getMemory());

    m_context.instance             = m_instance->get();
    m_context.device               = m_device->getDevice();
    m_context.physicalDevice       = m_device->getPhysicalDevice();
    m_context.surface              = m_device->getSurface();
    m_context.graphicsQueue        = m_device->getGraphicsQueue();
    m_context.presentQueue         = m_device->getPresentQueue();
    m_context.renderPass           = m_renderPass->get();
    m_context.swapchainExtent      = m_swapchain->getExtent();
    m_context.swapchainImageFormat = m_swapchain->getImageFormat();
}

Renderer::~Renderer() {
    waitIdle();

    // Resources tied to VkDevice
    m_vertexBuffer.reset();
    m_pipeline.reset();
    m_framebuffer.reset();
    m_renderPass.reset();
    m_syncObjects.reset();
    m_commandManager.reset();

    // Swapchain (uses device for destruction)
    m_swapchain.reset();

    // Device last
    m_device.reset();

    // Validation/debug afterwards (optional)
    if (m_config.enableValidationLayers) {
        m_debugMessenger.reset();
    }

    // Instance last
    m_instance.reset();

}



void Renderer::draw() {
    const size_t frameIndex = m_currentFrame;
    const auto& frameSync = m_syncObjects->getFrameSync(frameIndex);
    const auto& commandBuffers = m_commandManager->getCommandBuffers();
    VkDevice device = m_device->getDevice();
    VkSwapchainKHR swapchain = m_swapchain->get();
    VkRenderPass renderPass = m_renderPass->get();
    const VkExtent2D extent = m_swapchain->getExtent();
    const auto& framebuffers = m_framebuffer->getFramebuffers();

    // Wait for this frame’s fence
    vkWaitForFences(device, 1, &frameSync.inFlight, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &frameSync.inFlight);

    // Acquire image to render into
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frameSync.imageAvailable, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return; // Skip this frame
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }

    // Use command buffer for this frame, not image
    VkCommandBuffer cmd = commandBuffers[frameIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkClearValue clearColor = { .color = {{ 0.01f, 0.01, 0.01f, 1.0f }} };

    VkRenderPassBeginInfo renderPassInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = framebuffers[imageIndex],
        .renderArea = { {0, 0}, extent },
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Start GUI
    ImGuiLayer::beginFrame();
    ImGui::Begin("Debug Info");
    ImGui::Text("Hello from ImGui");
    ImGui::Text("Application FPS: %.0f", ImGui::GetIO().Framerate);
    ImGui::End();

    // Draw triangle
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->get());

    VkDeviceSize offset = 0;
    VkBuffer buffer = m_vertexBuffer->get();
    vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &offset);

    VkViewport viewport {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor {
        .offset = {0, 0},
        .extent = extent
    };

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);


    vkCmdDraw(cmd, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

    // End GUI
    ImGuiLayer::endFrame(cmd);

    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    // Submit
    VkSemaphore waitSemaphores[] = { frameSync.imageAvailable };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { frameSync.renderFinished };

    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };

    vkQueueSubmit(m_device->getGraphicsQueue(), 1, &submitInfo, frameSync.inFlight);

    // Present
    VkPresentInfoKHR presentInfo {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = &swapchain,
        .pImageIndices = &imageIndex
    };

    result = vkQueuePresentKHR(m_device->getPresentQueue(), &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized) {
        m_framebufferResized = false;
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("Failed to present swapchain image.");
    }

    // Advance frame
    m_currentFrame = (m_currentFrame + 1) % m_config.maxFramesInFlight;
}

void Renderer::onResize() {
    m_framebufferResized = true;
    // Wait for all operations to complete before recreating
    vkDeviceWaitIdle(m_device->getDevice());

    int width = 0, height = 0;
    glfwGetFramebufferSize(m_windowManager.get(), &width, &height);

    // Wait until window is non-zero
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_windowManager.get(), &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device->getDevice());

    // Destroy old
    m_framebuffer.reset();
    m_renderPass.reset();
    m_swapchain.reset();

    // Recreate
    const auto& indices = m_device->getQueueIndices();
    auto oldSwapchain = std::move(m_swapchain);

    m_swapchain = std::make_unique<VulkanSwapchain>(
        m_device->getPhysicalDevice(),
        m_device->getDevice(),
        m_config,
        m_device->getSurface(),
        indices.graphics.value(),
        indices.present.value(),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        oldSwapchain ? oldSwapchain->get() : VK_NULL_HANDLE
    );

    m_renderPass = std::make_unique<VulkanRenderPass>(
        m_device->getDevice(),
        m_swapchain->getImageFormat()
    );

    m_framebuffer = std::make_unique<VulkanFramebuffer>(
        m_device->getDevice(),
        m_renderPass->get(),
        m_swapchain->getImageViews(),
        m_swapchain->getExtent()
    );

    m_pipeline.reset();
    m_pipeline = std::make_unique<VulkanPipeline>(
        m_device->getDevice(),
        m_renderPass->get()
    );

}

void Renderer::waitIdle() {
    if (m_context.device)
        vkDeviceWaitIdle(m_context.device);
}

VulkanContext& Renderer::getContext() {
    return m_context;
}

uint32_t Renderer::getGraphicsQueueIndex() const {
    return m_device->getQueueIndices().graphics.value();
}

void Renderer::recreateSwapchain() {
    vkDeviceWaitIdle(m_context.device);

    int width = 0, height = 0;
    glfwGetFramebufferSize(m_windowManager.get(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_windowManager.get(), &width, &height);
        glfwWaitEvents();
    }

    // Destroy and reset relevant resources
    m_framebuffer.reset();
    m_renderPass.reset();
    m_swapchain.reset();
    m_pipeline.reset();
    m_syncObjects.reset();

    // Save old swapchain to allow reuse
    auto oldSwapchain = std::move(m_swapchain);

    const auto& indices = m_device->getQueueIndices();
    m_swapchain = std::make_unique<VulkanSwapchain>(
        m_context.physicalDevice,
        m_context.device,
        m_config, // ✅ reuse your existing config
        m_context.surface,
        indices.graphics.value(),
        indices.present.value(),
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height),
        oldSwapchain ? oldSwapchain->get() : VK_NULL_HANDLE
    );

    m_renderPass = std::make_unique<VulkanRenderPass>(
        m_context.device,
        m_swapchain->getImageFormat()
    );

    m_framebuffer = std::make_unique<VulkanFramebuffer>(
        m_context.device,
        m_renderPass->get(),
        m_swapchain->getImageViews(),
        m_swapchain->getExtent()
    );

    m_pipeline = std::make_unique<VulkanPipeline>(
        m_context.device,
        m_renderPass->get()
    );

    // ✅ Recreate sync objects after swapchain recreation
    m_syncObjects = std::make_unique<VulkanSyncObjects>(
        m_context.device,
        m_config.maxFramesInFlight
    );

    // ✅ Update context with the new swapchain values
    m_context.renderPass = m_renderPass->get();
    m_context.swapchainExtent = m_swapchain->getExtent();
    m_context.swapchainImageFormat = m_swapchain->getImageFormat();
}


