#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

class Camera {
public:
    virtual ~Camera() = default;

    virtual void update(float dt) = 0;
    virtual glm::mat4 getViewMatrix() const = 0;
    virtual glm::mat4 getProjectionMatrix() const = 0;

    virtual glm::vec3 getPosition() const = 0;
    virtual glm::quat getOrientation() const = 0;

};

#endif // CAMERA_H
