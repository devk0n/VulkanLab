#include "WindowManager.h"
#include <stdexcept>

WindowManager::WindowManager() {
    if (!glfwInit()) { throw std::runtime_error("Failed to initialize GLFW."); }
}

WindowManager::~WindowManager() {
    if (m_window) { glfwDestroyWindow(m_window); }
    glfwTerminate();
}

void WindowManager::create(uint32_t width, uint32_t height, const std::string& title) {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.c_str(), nullptr, nullptr);
    if (!m_window)
        throw std::runtime_error("Failed to create GLFW window.");

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}

void WindowManager::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self && self->m_resizeCallback) { self->m_resizeCallback(width, height); }
}
