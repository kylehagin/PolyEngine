#include "mesh.h"
#include "fs_util.h"
#include "log.h"
#include <cstring>

bool Mesh::Load(const std::filesystem::path& path, VkDevice device, VmaAllocator allocator, VkQueue, VkCommandPool)
{
    auto data = ReadFileBinary(path);
    if (data.size() < sizeof(MeshHeader)) return false;
    MeshHeader hdr;
    memcpy(&hdr, data.data(), sizeof(hdr));
    if (hdr.magic != 0x3048534D) { Log("Bad mesh magic"); return false; }
    indexCount = hdr.indexCount;
    // This simplified loader just creates buffers and uploads directly (no staging)
    VkBufferCreateInfo vb{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    vb.size = hdr.vertexCount * hdr.strideBytes;
    vb.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaAllocationCreateInfo vac{};
    vac.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    vmaCreateBuffer(allocator, &vb, &vac, &vertexBuffer, &vAlloc, nullptr);

    VkBufferCreateInfo ib{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    ib.size = hdr.indexCount * sizeof(uint32_t);
    ib.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    vmaCreateBuffer(allocator, &ib, &vac, &indexBuffer, &iAlloc, nullptr);

    // TODO: proper staging; for now assume host visible
    void* dst;
    vmaMapMemory(allocator, vAlloc, &dst);
    memcpy(dst, data.data() + sizeof(MeshHeader), hdr.vertexCount * hdr.strideBytes);
    vmaUnmapMemory(allocator, vAlloc);

    vmaMapMemory(allocator, iAlloc, &dst);
    memcpy(dst, data.data() + sizeof(MeshHeader) + hdr.vertexCount * hdr.strideBytes, hdr.indexCount * sizeof(uint32_t));
    vmaUnmapMemory(allocator, iAlloc);
    return true;
}

void Mesh::Destroy(VkDevice, VmaAllocator allocator)
{
    if (vertexBuffer) vmaDestroyBuffer(allocator, vertexBuffer, vAlloc);
    if (indexBuffer) vmaDestroyBuffer(allocator, indexBuffer, iAlloc);
}
