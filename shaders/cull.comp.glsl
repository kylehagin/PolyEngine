#version 460
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require
layout(local_size_x = 256) in;

struct InstanceData {
    mat4 model;
    vec4 sphere;
};

layout(std430, binding = 0) readonly buffer Instances {
    InstanceData instances[];
};

layout(std430, binding = 1) writeonly buffer Visible {
    uint visibleIndices[];
};

layout(std430, binding = 2) buffer IndirectOut {
    uint indexCount;
    uint instanceCount;
    uint firstIndex;
    int  vertexOffset;
    uint firstInstance;
};

layout(std430, binding = 3) readonly buffer CameraBuf {
    mat4 viewProj;
    vec4 frustum[6];
    vec4 eye;
};

layout(std430, binding = 4) buffer Counters {
    uint visibleCount;
};

layout(push_constant) uniform PushConsts {
    uint instanceTotal;
} pc;

void main()
{
    uint idx = gl_GlobalInvocationID.x;
    if (idx >= pc.instanceTotal) return;

    vec4 sph = instances[idx].sphere;
    bool visible = true;
    for (int i = 0; i < 6; ++i)
    {
        if (dot(frustum[i], vec4(sph.xyz,1.0)) < -sph.w)
        {
            visible = false;
            break;
        }
    }

    if (visible)
    {
        uint dst = atomicAdd(visibleCount, 1);
        visibleIndices[dst] = idx;
    }

    if (idx == 0) {
        instanceCount = visibleCount;
    }
}
