#pragma once
#include "vk_context.h"
#include "mesh.h"
#include "pipelines.h"
#include "camera.h"
#include <vector>

struct Renderer
{
    VkContext* ctx{};
    Pipelines pipes{};
    VkCommandPool cmdPool{};
    VkCommandBuffer cmd{};
    Mesh mesh;
    Camera camera;
    VkBuffer instanceBuf{};
    VmaAllocation instanceAlloc{};
    VkBuffer visibleBuf{};
    VmaAllocation visibleAlloc{};
    VkBuffer indirectBuf{};
    VmaAllocation indirectAlloc{};
    std::vector<InstanceData> instances;
    bool Init(VkContext& c, uint32_t width, uint32_t height);
    void Shutdown();
    void DrawFrame();
    void AddInstance(const InstanceData& inst);
};
