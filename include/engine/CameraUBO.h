#ifndef CAMERA_UBO_H
#define CAMERA_UBO_H

#include <glm/glm.hpp>

struct CameraUBO {
    glm::mat4 view;
    glm::mat4 projection;
};

#endif // CAMERA_UBO_H
