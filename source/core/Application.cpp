#include "Application.h"
#include "Logger.h"
#include "VulkanCommandManager.h"
#include "VulkanInstance.h"
#include "VulkanDebugMessenger.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanSyncObjects.h"

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

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(1280, 720, "VulkanLab", nullptr, nullptr);
    if (!m_window) {
        ERROR("Failed to create GLFW window.");
        throw std::runtime_error("Failed to create window.");
    }

    INFO("GLFW window created.");

    m_vulkanInstance = std::make_unique<VulkanInstance>(true);
    m_debugMessenger = std::make_unique<VulkanDebugMessenger>(m_vulkanInstance->get());
    m_device = std::make_unique<VulkanDevice>(m_vulkanInstance->get(), m_window);

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
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
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
        // Handle swapchain recreation here if needed
        return;
    }

    // Use command buffer for this frame, not image
    VkCommandBuffer cmd = commandBuffers[frameIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkClearValue clearColor = { .color = {{ 0.05f, 0.05f, 0.1f, 1.0f }} };

    VkRenderPassBeginInfo renderPassInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = framebuffers[imageIndex], // use imageIndex here!
        .renderArea = { {0, 0}, extent },
        .clearValueCount = 1,
        .pClearValues = &clearColor
    };

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
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

    vkQueuePresentKHR(m_device->getPresentQueue(), &presentInfo);

    // Advance frame
    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::cleanup() {
    if (m_device && m_device->getDevice()) {
        vkDeviceWaitIdle(m_device->getDevice());
    }

    m_syncObjects.reset();
    m_commandManager.reset();
    m_framebuffer.reset();
    m_renderPass.reset();
    m_swapchain.reset();
    m_device.reset();
    m_debugMessenger.reset();
    m_vulkanInstance.reset();

    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}
