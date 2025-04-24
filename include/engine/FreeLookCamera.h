#ifndef FREE_LOOK_CAMERA_H
#define FREE_LOOK_CAMERA_H

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class FreeLookCamera final : public Camera {
public:
    FreeLookCamera();

    void update(float dt) override;
    glm::mat4 getViewMatrix() const override;
    glm::mat4 getProjectionMatrix() const override;

    glm::vec3 getPosition() const override { return m_position; }
    glm::quat getOrientation() const override { return m_orientation; }

    void setPosition(const glm::vec3& pos) { m_position = pos; }

private:
    glm::vec3 m_position;
    glm::quat m_orientation;

    float m_moveSpeed = 4.5f;
    float m_mouseSensitivity = 0.0018f;
};

#endif // FREE_LOOK_CAMERA_H
