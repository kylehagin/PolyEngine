#include "vk_context.h"
#include "log.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <cstring>
#include <optional>

bool InitVulkan(VkContext& ctx, GLFWwindow* window, bool enableValidation)
{
    volkInitialize();

    VkApplicationInfo app{VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app.pApplicationName = "synty-lite";
    app.apiVersion = VK_API_VERSION_1_2;

    uint32_t extCount;
    const char** glfwExt = glfwGetRequiredInstanceExtensions(&extCount);
    std::vector<const char*> instExt(glfwExt, glfwExt + extCount);
    VkInstanceCreateInfo ci{VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    ci.pApplicationInfo = &app;
    ci.enabledExtensionCount = (uint32_t)instExt.size();
    ci.ppEnabledExtensionNames = instExt.data();
    if (enableValidation)
    {
        const char* layers[] = {"VK_LAYER_KHRONOS_validation"};
        ci.enabledLayerCount = 1;
        ci.ppEnabledLayerNames = layers;
    }
    if (vkCreateInstance(&ci, nullptr, &ctx.instance) != VK_SUCCESS) return false;
    volkLoadInstance(ctx.instance);

    if (glfwCreateWindowSurface(ctx.instance, window, nullptr, &ctx.surface) != VK_SUCCESS)
        return false;

    uint32_t physCount=0;
    vkEnumeratePhysicalDevices(ctx.instance, &physCount, nullptr);
    std::vector<VkPhysicalDevice> phys(physCount);
    vkEnumeratePhysicalDevices(ctx.instance, &physCount, phys.data());
    ctx.phys = phys[0];

    uint32_t queueCount=0;
    vkGetPhysicalDeviceQueueFamilyProperties(ctx.phys, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> qprops(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(ctx.phys, &queueCount, qprops.data());
    for(uint32_t i=0;i<queueCount;++i)
    {
        VkBool32 present;
        vkGetPhysicalDeviceSurfaceSupportKHR(ctx.phys, i, ctx.surface, &present);
        if ((qprops[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && present) {
            ctx.graphicsIdx = i;
            break;
        }
    }

    float prio = 1.0f;
    VkDeviceQueueCreateInfo qci{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    qci.queueFamilyIndex = ctx.graphicsIdx;
    qci.queueCount = 1;
    qci.pQueuePriorities = &prio;

    VkDeviceCreateInfo dci{VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};
    dci.queueCreateInfoCount = 1;
    dci.pQueueCreateInfos = &qci;
    const char* devExt[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    dci.enabledExtensionCount = 1;
    dci.ppEnabledExtensionNames = devExt;
    if (enableValidation)
    {
        const char* layers[] = {"VK_LAYER_KHRONOS_validation"};
        dci.enabledLayerCount = 1;
        dci.ppEnabledLayerNames = layers;
    }
    if (vkCreateDevice(ctx.phys, &dci, nullptr, &ctx.device) != VK_SUCCESS) return false;
    volkLoadDevice(ctx.device);
    vkGetDeviceQueue(ctx.device, ctx.graphicsIdx, 0, &ctx.graphicsQ);

    VmaAllocatorCreateInfo aci{};
    aci.instance = ctx.instance;
    aci.physicalDevice = ctx.phys;
    aci.device = ctx.device;
    vmaCreateAllocator(&aci, &ctx.allocator);

    return true;
}

void ShutdownVulkan(VkContext& ctx)
{
    vmaDestroyAllocator(ctx.allocator);
    vkDestroyDevice(ctx.device, nullptr);
    vkDestroySurfaceKHR(ctx.instance, ctx.surface, nullptr);
    vkDestroyInstance(ctx.instance, nullptr);
}
