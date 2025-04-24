#include "FreeLookCamera.h"
#include "InputManager.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

FreeLookCamera::FreeLookCamera()
    : m_position(0.0f), m_orientation(1, 0, 0, 0) {}

void FreeLookCamera::update(float dt) {
    using namespace glm;

    // Mouse rotation: yaw (around Z+), pitch (around Y+)
    const vec2 delta = InputManager::getMouseDelta();
    quat yaw   = angleAxis(-delta.x * m_mouseSensitivity, vec3(0, 0, 1)); // yaw around Z+ (up)
    quat pitch = angleAxis(-delta.y * m_mouseSensitivity, vec3(0, 1, 0)); // pitch around Y+ (left)

    m_orientation = normalize(yaw * m_orientation * pitch);

    // Movement axes — matching your custom basis
    vec3 forward = m_orientation * vec3(1, 0, 0); // X+ forward
    vec3 left    = m_orientation * vec3(0, 1, 0); // Y+ left
    vec3 up      = m_orientation * vec3(0, 0, 1); // Z+ up

    vec3 velocity(0.0f);
    if (InputManager::isKeyDown(GLFW_KEY_W)) velocity += forward;
    if (InputManager::isKeyDown(GLFW_KEY_S)) velocity -= forward;
    if (InputManager::isKeyDown(GLFW_KEY_A)) velocity += left;
    if (InputManager::isKeyDown(GLFW_KEY_D)) velocity -= left;
    if (InputManager::isKeyDown(GLFW_KEY_E)) velocity += up;
    if (InputManager::isKeyDown(GLFW_KEY_Q)) velocity -= up;

    m_position += velocity * m_moveSpeed * dt;
}

glm::mat4 FreeLookCamera::getViewMatrix() const {
    glm::vec3 forward = m_orientation * glm::vec3(1, 0, 0); // X+
    glm::vec3 up      = m_orientation * glm::vec3(0, 0, 1); // Z+
    return lookAt(m_position, m_position + forward, up);
}

glm::mat4 FreeLookCamera::getProjectionMatrix() const {
    float aspectRatio = 2048 / 1152;
    return glm::perspective(glm::radians(90.0f), aspectRatio, 0.1f, 1000.0f);
}

