#include "InputManager.h"
#include <imgui.h>

GLFWwindow* InputManager::s_window = nullptr;
std::unordered_map<int, bool> InputManager::s_keys;
std::unordered_map<int, bool> InputManager::s_prevKeys;

std::unordered_map<int, bool> InputManager::s_mouseButtons;
std::unordered_map<int, bool> InputManager::s_prevMouseButtons;

glm::vec2 InputManager::s_mousePos = {};
glm::vec2 InputManager::s_lastMousePos = {};
glm::vec2 InputManager::s_mouseDelta = {};

bool InputManager::s_ignoreImGui = true;

void InputManager::initialize(GLFWwindow* window) {
    s_window = window;
    glfwGetCursorPos(s_window, reinterpret_cast<double *>(&s_mousePos.x), reinterpret_cast<double *>(&s_mousePos.y));
    s_lastMousePos = s_mousePos;
}

void InputManager::update() {
    // Store previous states
    s_prevKeys = s_keys;
    s_prevMouseButtons = s_mouseButtons;
    s_lastMousePos = s_mousePos;

    // Mouse position
    double x, y;
    glfwGetCursorPos(s_window, &x, &y);
    s_mousePos = { static_cast<float>(x), static_cast<float>(y) };
    s_mouseDelta = s_mousePos - s_lastMousePos;

    // Keys
    for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        s_keys[key] = glfwGetKey(s_window, key) == GLFW_PRESS;

    // Mouse buttons
    for (int button = GLFW_MOUSE_BUTTON_1; button <= GLFW_MOUSE_BUTTON_LAST; ++button)
        s_mouseButtons[button] = glfwGetMouseButton(s_window, button) == GLFW_PRESS;
}

bool InputManager::isKeyDown(int key) {
    if (s_ignoreImGui && ImGui::GetIO().WantCaptureKeyboard) return false;
    return s_keys[key];
}

bool InputManager::isKeyPressed(int key) {
    if (s_ignoreImGui && ImGui::GetIO().WantCaptureKeyboard) return false;
    return s_keys[key] && !s_prevKeys[key];
}

bool InputManager::isKeyReleased(int key) {
    if (s_ignoreImGui && ImGui::GetIO().WantCaptureKeyboard) return false;
    return !s_keys[key] && s_prevKeys[key];
}

bool InputManager::isMouseDown(int button) {
    if (s_ignoreImGui && ImGui::GetIO().WantCaptureMouse) return false;
    return s_mouseButtons[button];
}

bool InputManager::isMousePressed(int button) {
    if (s_ignoreImGui && ImGui::GetIO().WantCaptureMouse) return false;
    return s_mouseButtons[button] && !s_prevMouseButtons[button];
}

bool InputManager::isMouseReleased(int button) {
    if (s_ignoreImGui && ImGui::GetIO().WantCaptureMouse) return false;
    return !s_mouseButtons[button] && s_prevMouseButtons[button];
}

glm::vec2 InputManager::getMouseDelta() {
    if (s_ignoreImGui && ImGui::GetIO().WantCaptureMouse) return {};
    return s_mouseDelta;
}

glm::vec2 InputManager::getMousePosition() {
    return s_mousePos;
}

void InputManager::setIgnoreImGui(bool value) {
    s_ignoreImGui = value;
}
