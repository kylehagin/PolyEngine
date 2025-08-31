#include "pipelines.h"
#include "fs_util.h"
#include "log.h"

static VkShaderModule LoadModule(VkDevice dev, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo ci{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    ci.codeSize = code.size();
    ci.pCode = reinterpret_cast<const uint32_t*>(code.data());
    VkShaderModule mod;
    vkCreateShaderModule(dev, &ci, nullptr, &mod);
    return mod;
}

bool CreatePipelines(VkContext& ctx, Pipelines& pipe)
{
    // Descriptor layouts
    VkDescriptorSetLayoutBinding compBinds[5]{};
    for (int i=0;i<5;++i){ compBinds[i].binding=i; compBinds[i].descriptorCount=1; compBinds[i].stageFlags=VK_SHADER_STAGE_COMPUTE_BIT; }
    compBinds[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    compBinds[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    compBinds[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    compBinds[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    compBinds[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    VkDescriptorSetLayoutCreateInfo compSL{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    compSL.bindingCount = 5; compSL.pBindings = compBinds;
    vkCreateDescriptorSetLayout(ctx.device, &compSL, nullptr, &pipe.computeSet);

    VkDescriptorSetLayoutBinding gfxBinds[4]{};
    for (int i=0;i<4;++i){ gfxBinds[i].binding=i; gfxBinds[i].descriptorCount=1; gfxBinds[i].descriptorType=(i==3?VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:VK_DESCRIPTOR_TYPE_STORAGE_BUFFER); gfxBinds[i].stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT; }
    VkDescriptorSetLayoutCreateInfo gfxSL{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    gfxSL.bindingCount = 4; gfxSL.pBindings = gfxBinds;
    vkCreateDescriptorSetLayout(ctx.device, &gfxSL, nullptr, &pipe.graphicsSet);

    VkPipelineLayoutCreateInfo pcl{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    pcl.setLayoutCount = 1; pcl.pSetLayouts = &pipe.computeSet;
    VkPushConstantRange pc{VK_SHADER_STAGE_COMPUTE_BIT,0,sizeof(uint32_t)};
    pcl.pushConstantRangeCount=1;pcl.pPushConstantRanges=&pc;
    vkCreatePipelineLayout(ctx.device, &pcl, nullptr, &pipe.computeLayout);

    VkPipelineLayoutCreateInfo gl{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    gl.setLayoutCount = 1; gl.pSetLayouts = &pipe.graphicsSet;
    vkCreatePipelineLayout(ctx.device, &gl, nullptr, &pipe.graphicsLayout);

    // Compute pipeline
    auto cullCode = ReadFileBinary("shaders/cull.comp.glsl.spv");
    VkShaderModule cullMod = LoadModule(ctx.device, cullCode);
    VkComputePipelineCreateInfo cci{VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO};
    cci.stage = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    cci.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    cci.stage.module = cullMod;
    cci.stage.pName = "main";
    cci.layout = pipe.computeLayout;
    vkCreateComputePipelines(ctx.device, VK_NULL_HANDLE, 1, &cci, nullptr, &pipe.computePipeline);
    vkDestroyShaderModule(ctx.device, cullMod, nullptr);

    // Graphics pipeline (basic)
    auto vsCode = ReadFileBinary("shaders/instanced.vert.glsl.spv");
    auto fsCode = ReadFileBinary("shaders/flat.frag.glsl.spv");
    VkShaderModule vs = LoadModule(ctx.device, vsCode);
    VkShaderModule fs = LoadModule(ctx.device, fsCode);

    VkPipelineShaderStageCreateInfo stages[2]{};
    stages[0] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vs;
    stages[0].pName = "main";
    stages[1] = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = fs;
    stages[1].pName = "main";

    VkVertexInputBindingDescription vbind{0, sizeof(float)*8, VK_VERTEX_INPUT_RATE_VERTEX};
    VkVertexInputAttributeDescription attrs[3] = {
        {0,0,VK_FORMAT_R32G32B32_SFLOAT,0},
        {1,0,VK_FORMAT_R32G32B32_SFLOAT,12},
        {2,0,VK_FORMAT_R32G32_SFLOAT,24}
    };
    VkPipelineVertexInputStateCreateInfo vi{VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vi.vertexBindingDescriptionCount=1;vi.pVertexBindingDescriptions=&vbind;
    vi.vertexAttributeDescriptionCount=3;vi.pVertexAttributeDescriptions=attrs;

    VkPipelineInputAssemblyStateCreateInfo ia{VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport vp{};
    vp.width = 800; vp.height = 600; vp.maxDepth=1;
    VkRect2D sc{{0,0},{800,600}};
    VkPipelineViewportStateCreateInfo vpstate{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    vpstate.viewportCount=1;vpstate.pViewports=&vp;
    vpstate.scissorCount=1;vpstate.pScissors=&sc;

    VkPipelineRasterizationStateCreateInfo rs{VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rs.polygonMode=VK_POLYGON_MODE_FILL;
    rs.cullMode=VK_CULL_MODE_BACK_BIT;
    rs.frontFace=VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rs.lineWidth=1.0f;

    VkPipelineMultisampleStateCreateInfo ms{VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    ms.rasterizationSamples=VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState cbatt{};
    cbatt.colorWriteMask = 0xF;
    VkPipelineColorBlendStateCreateInfo cb{VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    cb.attachmentCount=1;cb.pAttachments=&cbatt;

    VkPipelineDepthStencilStateCreateInfo ds{VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    ds.depthTestEnable = VK_TRUE;
    ds.depthWriteEnable = VK_TRUE;
    ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;

    VkGraphicsPipelineCreateInfo gpci{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    gpci.stageCount=2;gpci.pStages=stages;
    gpci.pVertexInputState=&vi;
    gpci.pInputAssemblyState=&ia;
    gpci.pViewportState=&vpstate;
    gpci.pRasterizationState=&rs;
    gpci.pMultisampleState=&ms;
    gpci.pColorBlendState=&cb;
    gpci.pDepthStencilState=&ds;
    gpci.layout = pipe.graphicsLayout;
    gpci.renderPass = VK_NULL_HANDLE; // set later by renderer
    gpci.subpass = 0;
    vkCreateGraphicsPipelines(ctx.device, VK_NULL_HANDLE, 1, &gpci, nullptr, &pipe.graphicsPipeline);

    vkDestroyShaderModule(ctx.device, vs, nullptr);
    vkDestroyShaderModule(ctx.device, fs, nullptr);

    VkDescriptorPoolSize sizes[2] = {
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 8},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4}
    };
    VkDescriptorPoolCreateInfo dpci{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    dpci.maxSets = 8; dpci.poolSizeCount=2; dpci.pPoolSizes=sizes;
    vkCreateDescriptorPool(ctx.device, &dpci, nullptr, &pipe.descriptorPool);
    return true;
}

void DestroyPipelines(VkContext& ctx, Pipelines& pipe)
{
    vkDestroyDescriptorPool(ctx.device, pipe.descriptorPool, nullptr);
    vkDestroyDescriptorSetLayout(ctx.device, pipe.computeSet, nullptr);
    vkDestroyDescriptorSetLayout(ctx.device, pipe.graphicsSet, nullptr);
    vkDestroyPipeline(ctx.device, pipe.computePipeline, nullptr);
    vkDestroyPipeline(ctx.device, pipe.graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(ctx.device, pipe.computeLayout, nullptr);
    vkDestroyPipelineLayout(ctx.device, pipe.graphicsLayout, nullptr);
}
