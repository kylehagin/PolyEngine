#pragma once
#include <volk.h>
#include <vector>
#include "vk_mem_alloc.h"

struct VkContext
{
    VkInstance instance{};
    VkPhysicalDevice phys{};
    VkDevice device{};
    VkQueue graphicsQ{};
    uint32_t graphicsIdx = 0;
    VkSurfaceKHR surface{};
    VkSwapchainKHR swapchain{};
    VkFormat swapFormat{};
    VkExtent2D swapExtent{};
    std::vector<VkImage> swapImages;
    std::vector<VkImageView> swapViews;
    VmaAllocator allocator{};
};

bool InitVulkan(VkContext& ctx, GLFWwindow* window, bool enableValidation);
void ShutdownVulkan(VkContext& ctx);
