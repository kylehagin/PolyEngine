#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct InstanceData {
    glm::mat4 model;
    glm::vec4 sphere; // xyz=center, w=radius
};

struct CameraGPU {
    glm::mat4 viewProj;
    glm::vec4 frustum[6];
    glm::vec4 eye;
};

struct DrawIndexedIndirect {
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  vertexOffset;
    uint32_t firstInstance;
};
