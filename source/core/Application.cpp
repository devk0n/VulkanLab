#include "Application.h"
#include "Logger.h"
#include "VulkanInstance.h"
#include "VulkanDebugMessenger.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"

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

    m_window = glfwCreateWindow(1280, 720, "Fracture", nullptr, nullptr);
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
}

void Application::mainLoop() {
    while (!glfwWindowShouldClose(m_window)) {
        glfwPollEvents();
        // Draw frame here later
    }
}

void Application::cleanup() {
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