#ifndef FREE_LOOK_CAMERA_H
#define FREE_LOOK_CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.h"

class FreeLookCamera final : public Camera {
public:
    FreeLookCamera(float fov, float aspect, float nearPlane, float farPlane);
    void update(float dt) override;
    glm::mat4 getViewMatrix() const override;
    glm::mat4 getProjectionMatrix() const override;

    void processInput(GLFWwindow* window, float dt);
private:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;

    float m_yaw, m_pitch;
    float m_fov, m_aspect, m_near, m_far;
};

#endif // FREE_LOOK_CAMERA_H
