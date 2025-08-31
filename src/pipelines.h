#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include "vk_context.h"

struct Pipelines
{
    VkPipelineLayout computeLayout{};
    VkPipeline computePipeline{};

    VkPipelineLayout graphicsLayout{};
    VkPipeline graphicsPipeline{};

    VkDescriptorSetLayout computeSet{};
    VkDescriptorSetLayout graphicsSet{};
    VkDescriptorPool descriptorPool{};
};

bool CreatePipelines(VkContext& ctx, Pipelines& pipe);
void DestroyPipelines(VkContext& ctx, Pipelines& pipe);
