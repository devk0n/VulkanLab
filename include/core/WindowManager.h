#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <GLFW/glfw3.h>
#include <string>
#include <functional>

class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    void create(uint32_t width, uint32_t height, const std::string& title);

    static void pollEvents() { glfwPollEvents(); }
    [[nodiscard]] bool shouldClose() const { return glfwWindowShouldClose(m_window); }
    void setResizeCallback(std::function<void(int, int)> callback) { m_resizeCallback = std::move(callback); }
    [[nodiscard]] GLFWwindow* get() const { return m_window; }

private:
    GLFWwindow* m_window = nullptr;
    std::function<void(int, int)> m_resizeCallback;

    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};


#endif // WINDOW_MANAGER_H
