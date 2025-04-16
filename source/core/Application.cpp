#include "Application.h"

#include <cstring>
#include <imgui.h>

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


std::vector<Vertex> vertices = {
    { {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },  // Bottom
    { {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },  // Right
    { { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }   // Left
};

Application::Application() {
    initialize();
}

Application::~Application() {
    cleanup();
}

void Application::run() {
    mainLoop();
}

void Application::initialize() {
    if (!glfwInit()) {
        ERROR("GLFW initialization failed.");
        throw std::runtime_error("Failed to initialize GLFW.");
    }

    m_windowManager = std::make_unique<WindowManager>();
    m_windowManager->create(1280, 720, "VulkanLab");

    m_windowManager->setResizeCallback([this](int w, int h) {
        m_framebufferResized = true;
    });

    m_vulkanInstance = std::make_unique<VulkanInstance>(true);
    m_debugMessenger = std::make_unique<VulkanDebugMessenger>(m_vulkanInstance->get());
    m_device = std::make_unique<VulkanDevice>(m_vulkanInstance->get(), *m_windowManager);

    const auto& indices = m_device->getQueueIndices();
    m_swapchain = std::make_unique<VulkanSwapchain>(
        m_device->getPhysicalDevice(),
        m_device->getDevice(),
        m_device->getSurface(),
        indices.graphics.value(),
        indices.present.value(),
        1280, 720
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

    m_commandManager = std::make_unique<VulkanCommandManager>(
        m_device->getDevice(),
        m_device->getQueueIndices().graphics.value(),
        MAX_FRAMES_IN_FLIGHT
    );

    m_syncObjects = std::make_unique<VulkanSyncObjects>(
        m_device->getDevice(),
        MAX_FRAMES_IN_FLIGHT
    );

    m_pipeline = std::make_unique<VulkanPipeline>(
        m_device->getDevice(),
        m_renderPass->get()
    );

    m_imguiLayer = std::make_unique<ImGuiLayer>(
        m_windowManager->get(),
        m_vulkanInstance->get(),
        m_device->getDevice(),
        m_device->getPhysicalDevice(),
        m_device->getGraphicsQueue(),
        m_device->getQueueIndices().graphics.value(),
        m_renderPass->get(),
        static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT)
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

}

void Application::mainLoop() {
    while (!m_windowManager->shouldClose()) {
        glfwPollEvents();
        drawFrame();
    }
}

void Application::drawFrame() {
    const size_t frameIndex = m_currentFrame;
    const auto& frameSync = m_syncObjects->getFrameSync(frameIndex);
    const auto& commandBuffers = m_commandManager->getCommandBuffers();
    const VkDevice device = m_device->getDevice();
    const VkSwapchainKHR swapchain = m_swapchain->get();
    const VkRenderPass renderPass = m_renderPass->get();
    const VkExtent2D extent = m_swapchain->getExtent();
    const auto& framebuffers = m_framebuffer->getFramebuffers();

    // Wait for this frame’s fence
    vkWaitForFences(device, 1, &frameSync.inFlight, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &frameSync.inFlight);

    // Acquire image to render into
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX,
                                        frameSync.imageAvailable, VK_NULL_HANDLE, &imageIndex);


    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapchain();
        return; // Skip this frame
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swapchain image.");
    }

    // Use command buffer for this frame, not image
    VkCommandBuffer cmd = commandBuffers[frameIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkClearValue clearColor = { .color = {{ 0.0f, 0.0f, 0.0f, 1.0f }} };

    VkRenderPassBeginInfo renderPassInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = framebuffers[imageIndex], // use imageIndex here!
        .renderArea = { {0, 0}, extent },
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // Start GUI
    m_imguiLayer->beginFrame();
    ImGui::Begin("Debug Info");
    ImGui::Text("Hello from ImGui");
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
    m_imguiLayer->endFrame(cmd);

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
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::cleanup() {
    if (m_device) {
        vkDeviceWaitIdle(m_device->getDevice());
    }

    // Destroy objects in reverse creation order
    m_vertexBuffer.reset();     // Must go before device
    m_pipeline.reset();         // Before render pass and device
    m_syncObjects.reset();      // Before device
    m_commandManager.reset();   // Before device
    m_framebuffer.reset();      // Before render pass
    m_renderPass.reset();       // Before device
    m_swapchain.reset();        // Before device
    m_imguiLayer.reset();
    m_device.reset();           // Now it's safe to destroy the device
    m_debugMessenger.reset();   // Instance still valid here
    m_vulkanInstance.reset();   // Last to be destroyed
    m_windowManager.reset();    // Must go last, since it destroys the window

    glfwTerminate();
}

void Application::recreateSwapchain() {
    // Wait for all operations to complete before recreating
    vkDeviceWaitIdle(m_device->getDevice());

    int width = 0, height = 0;
    glfwGetFramebufferSize(m_windowManager->get(), &width, &height);

    // Wait until window is non-zero
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_windowManager->get(), &width, &height);
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

void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->m_framebufferResized = true;
}
