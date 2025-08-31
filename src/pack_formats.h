#pragma once
#include <cstdint>

struct MeshHeader {
    uint32_t magic;       // 'MSH0'
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t strideBytes; // sizeof(float3 pos, float3 nrm, float2 uv) = 32
};

// Interleaved vertex: float3 position, float3 normal, float2 uv
