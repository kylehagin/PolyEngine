#pragma once
#include <cstdint>
#include <filesystem>
#include "math.h"
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

struct MeshHeader {
    uint32_t magic;
    uint32_t vertexCount;
    uint32_t indexCount;
    uint32_t strideBytes;
};

class Mesh
{
public:
    bool Load(const std::filesystem::path& path, VkDevice device, VmaAllocator allocator, VkQueue transferQ, VkCommandPool pool);
    void Destroy(VkDevice device, VmaAllocator allocator);
    VkBuffer vertexBuffer {VK_NULL_HANDLE};
    VkBuffer indexBuffer  {VK_NULL_HANDLE};
    uint32_t indexCount = 0;
private:
    VmaAllocation vAlloc{}, iAlloc{};
};
