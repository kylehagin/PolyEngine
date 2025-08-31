#include "renderer.h"
#include "log.h"
#include "fs_util.h"
#include "unit_cube.h"
#include <filesystem>

bool Renderer::Init(VkContext& c, uint32_t width, uint32_t height)
{
    ctx = &c;
    CreatePipelines(*ctx, pipes);

    VkCommandPoolCreateInfo pci{VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    pci.queueFamilyIndex = ctx->graphicsIdx;
    vkCreateCommandPool(ctx->device, &pci, nullptr, &cmdPool);

    VkCommandBufferAllocateInfo ai{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    ai.commandPool = cmdPool;
    ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = 1;
    vkAllocateCommandBuffers(ctx->device, &ai, &cmd);

    camera.Init(width/(float)height);

    // allocate buffers (simplified host visible)
    VkBufferCreateInfo bi{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bi.size = sizeof(InstanceData) * 1024;
    bi.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    VmaAllocationCreateInfo ac{};
    ac.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
    vmaCreateBuffer(ctx->allocator, &bi, &ac, &instanceBuf, &instanceAlloc, nullptr);

    bi.size = sizeof(uint32_t) * 1024;
    vmaCreateBuffer(ctx->allocator, &bi, &ac, &visibleBuf, &visibleAlloc, nullptr);

    bi.size = sizeof(DrawIndexedIndirect);
    bi.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    vmaCreateBuffer(ctx->allocator, &bi, &ac, &indirectBuf, &indirectAlloc, nullptr);

    // ensure default mesh exists and load it
    EnsureDir("assets");
    if (!std::filesystem::exists("assets/unit_cube.bin"))
        WriteUnitCubeBin("assets/unit_cube.bin");
    mesh.Load("assets/unit_cube.bin", ctx->device, ctx->allocator, ctx->graphicsQ, cmdPool);

    // default instance
    InstanceData inst{};
    inst.model = glm::mat4(1);
    inst.sphere = glm::vec4(0,0,0,0.5f);
    AddInstance(inst);
    return true;
}

void Renderer::Shutdown()
{
    mesh.Destroy(ctx->device, ctx->allocator);
    vmaDestroyBuffer(ctx->allocator, instanceBuf, instanceAlloc);
    vmaDestroyBuffer(ctx->allocator, visibleBuf, visibleAlloc);
    vmaDestroyBuffer(ctx->allocator, indirectBuf, indirectAlloc);
    DestroyPipelines(*ctx, pipes);
    vkDestroyCommandPool(ctx->device, cmdPool, nullptr);
}

void Renderer::AddInstance(const InstanceData& inst)
{
    instances.push_back(inst);
    void* dst;
    vmaMapMemory(ctx->allocator, instanceAlloc, &dst);
    memcpy(dst, instances.data(), instances.size()*sizeof(InstanceData));
    vmaUnmapMemory(ctx->allocator, instanceAlloc);
}

void Renderer::DrawFrame()
{
    camera.Update(0.016f);
    void* dst;
    vmaMapMemory(ctx->allocator, instanceAlloc, &dst);
    memcpy(dst, instances.data(), instances.size()*sizeof(InstanceData));
    vmaUnmapMemory(ctx->allocator, instanceAlloc);

    vkResetCommandPool(ctx->device, cmdPool, 0);
    VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(cmd, &bi);
    // The full rendering sequence (swapchain, render pass, culling dispatch)
    // is omitted for brevity – this sample only demonstrates command buffer setup.
    vkEndCommandBuffer(cmd);

    VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    si.commandBufferCount=1;si.pCommandBuffers=&cmd;
    vkQueueSubmit(ctx->graphicsQ,1,&si,VK_NULL_HANDLE);
    vkQueueWaitIdle(ctx->graphicsQ);
}
