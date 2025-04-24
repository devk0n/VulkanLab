#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GLFW/glfw3.h>
#include <unordered_map>
#include <glm/glm.hpp>

class InputManager {
public:
    static void initialize(GLFWwindow* window);
    static void update(); // call each frame

    static bool isKeyDown(int key);
    static bool isKeyPressed(int key);
    static bool isKeyReleased(int key);

    static bool isMouseDown(int button);
    static bool isMousePressed(int button);
    static bool isMouseReleased(int button);
    static glm::vec2 getMouseDelta();
    static glm::vec2 getMousePosition();

    static void setIgnoreImGui(bool value);

private:
    static GLFWwindow* s_window;
    static std::unordered_map<int, bool> s_keys;
    static std::unordered_map<int, bool> s_prevKeys;

    static std::unordered_map<int, bool> s_mouseButtons;
    static std::unordered_map<int, bool> s_prevMouseButtons;

    static glm::vec2 s_mousePos;
    static glm::vec2 s_lastMousePos;
    static glm::vec2 s_mouseDelta;

    static bool s_ignoreImGui;
};

#endif // INPUT_MANAGER_H
