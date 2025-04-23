#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <GLFW/glfw3.h>
#include <string>
#include <functional>
#include <vulkan/vulkan.h>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    void create(const std::string& title);
    static void pollEvents() { glfwPollEvents(); }
    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(m_window); }

    [[nodiscard]] GLFWwindow* get() const { return m_window; }
    [[nodiscard]] int getWidth() const { return m_width; }
    [[nodiscard]] int getHeight() const { return m_height; }

    [[nodiscard]] VkExtent2D getExtent() const { return {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}; }

    void setResizeCallback(std::function<void(int, int)> callback) { m_resizeCallback = std::move(callback); }

private:
    GLFWwindow* m_window = nullptr;
    int m_width = 1280;
    int m_height = 960;
    std::function<void(int, int)> m_resizeCallback;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};

#endif // WINDOW_MANAGER_H
