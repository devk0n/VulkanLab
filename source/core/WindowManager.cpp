#include "WindowManager.h"
#include <stdexcept>

WindowManager::WindowManager() {
    if (!glfwInit()) { throw std::runtime_error("Failed to initialize GLFW."); }
}

WindowManager::~WindowManager() {
    if (m_window) { glfwDestroyWindow(m_window); }
    glfwTerminate();
}

void WindowManager::create(const std::string& title) {
    // Get primary monitor video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Set width and height to 80% of screen size
    m_width = static_cast<int>(mode->width * 0.8);
    m_height = static_cast<int>(mode->height * 0.8);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        throw std::runtime_error("Failed to create GLFW window.");
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);
}


void WindowManager::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto* self = static_cast<WindowManager*>(glfwGetWindowUserPointer(window));
    if (self && self->m_resizeCallback) { self->m_resizeCallback(width, height); }
}
