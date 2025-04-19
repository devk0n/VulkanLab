#ifndef APPLICATION_H
#define APPLICATION_H

#include <memory>

class WindowManager;
class Renderer;
class ImGuiLayer;

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    std::unique_ptr<WindowManager> m_windowManager;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<ImGuiLayer> m_imguiLayer;

};

#endif // APPLICATION_H
