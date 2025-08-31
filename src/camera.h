#pragma once
#include "math.h"

class Camera
{
public:
    void Init(float aspect);
    void Update(float dt);
    const CameraGPU& GetGPU() const { return gpu; }
    const glm::mat4& GetViewProj() const { return gpu.viewProj; }
private:
    void Rebuild();
    CameraGPU gpu {};
    glm::vec3 pos {0,1,5};
    glm::vec3 rot {0,0,0};
    float aspect = 1.0f;
};
