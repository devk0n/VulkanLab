#ifndef VERTEX_H
#define VERTEX_H

#include <array>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        return {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        };
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        return {{
            {
                .location = 0,
                .binding = 0,
                .format = VK_FORMAT_R32G32_SFLOAT, // vec2
                .offset = offsetof(Vertex, position)
            },
            {
                .location = 1,
                .binding = 0,
                .format = VK_FORMAT_R32G32B32_SFLOAT, // vec3
                .offset = offsetof(Vertex, color)
            }
        }};
    }
};

#endif // VERTEX_H
