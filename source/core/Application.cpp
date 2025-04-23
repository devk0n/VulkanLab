#include "Application.h"
#include "WindowManager.h"
#include "Renderer.h"
#include "ImGuiLayer.h"
#include "Logger.h"

Application::Application() {
    m_windowManager = std::make_unique<WindowManager>();
    m_windowManager->create(1280, 720, "VulkanLab");

    m_renderer = std::make_unique<Renderer>(*m_windowManager);

    m_imguiLayer = std::make_unique<ImGuiLayer>(
        m_windowManager->get(),
        m_renderer->getContext(),
        m_renderer->getConfig()
    );

    m_windowManager->setResizeCallback([this](int, int) {
        m_renderer->onResize();
    });
}

Application::~Application() {
    m_imguiLayer.reset();
    m_renderer.reset();
    m_windowManager.reset();
}

void Application::run() {
    while (!m_windowManager->shouldClose()) {
        glfwPollEvents();
        m_renderer->draw();
    }
}

