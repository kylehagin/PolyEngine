#version 460

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNrm;
layout(location = 2) in vec2 inUV;

struct InstanceData {
    mat4 model;
    vec4 sphere;
};

layout(std430, binding = 0) readonly buffer Instances {
    InstanceData instances[];
};

layout(std430, binding = 1) readonly buffer Visible {
    uint visibleIndices[];
};

layout(std140, binding = 2) uniform CameraUBO {
    mat4 viewProj;
    vec4 frustum[6];
    vec4 eye;
} camera;

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec3 vNrm;

void main()
{
    uint src = visibleIndices[gl_InstanceIndex];
    mat4 model = instances[src].model;
    vec4 worldPos = model * vec4(inPos,1.0);
    vNrm = mat3(model) * inNrm;
    gl_Position = camera.viewProj * worldPos;
}
