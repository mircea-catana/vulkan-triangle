#include <windows.h>

#define VK_USE_PLATFORM_WIN32_KHR
#define VK_NO_PROTOTYPES
#include "vulkan.h"

struct vulkan_context {
    uint32_t width;
    uint32_t height;

    VkInstance instance;
    VkSurfaceKHR surface;

    uint32_t presentQueueIndex;
    VkQueue presentQueue;

    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties memoryProperties;

    VkDevice device;

    VkCommandBuffer setupCmdBuffer;
    VkCommandBuffer drawCmdBuffer;

    VkSwapchainKHR swapchain;

    VkImage *presentImages;
    VkImage depthImage;
    VkImageView depthImageView;
    VkFramebuffer *frameBuffers;

    VkRenderPass renderpass;

    VkBuffer vertexInputBuffer;

    VkPipeline pipeline;
    VkPipelineLayout pipelineLayout;

    VkDebugReportCallbackEXT debugCb;

} context;

void assert(bool flag, char *msg = "") {
    if (!flag) {
        OutputDebugStringA("ASSERT: ");
        OutputDebugStringA(msg);
        OutputDebugStringA("\n");
        exit(1337);
    }
}

wchar_t* convertToLPCWSTR(const char *charArray) {
    wchar_t *wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}

void checkVkResult(VkResult &result, char *msg) {
    assert(VK_SUCCESS == result, msg);
}

// Core
PFN_vkCreateInstance vkCreateInstance = NULL;
PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties = NULL;
PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties = NULL;
PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = NULL;
PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = NULL;
PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = NULL;
PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = NULL;
PFN_vkCreateDevice vkCreateDevice = NULL;
PFN_vkGetDeviceQueue vkGetDeviceQueue = NULL;
PFN_vkCreateCommandPool vkCreateCommandPool = NULL;
PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers = NULL;
PFN_vkCreateFence vkCreateFence = NULL;
PFN_vkBeginCommandBuffer vkBeginCommandBuffer = NULL;
PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier = NULL;
PFN_vkEndCommandBuffer vkEndCommandBuffer = NULL;
PFN_vkQueueSubmit vkQueueSubmit = NULL;
PFN_vkWaitForFences vkWaitForFences = NULL;
PFN_vkResetFences vkResetFences = NULL;
PFN_vkResetCommandBuffer vkResetCommandBuffer = NULL;
PFN_vkCreateImageView vkCreateImageView = NULL;
PFN_vkCreateImage vkCreateImage = NULL;
PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements = NULL;
PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = NULL;
PFN_vkAllocateMemory vkAllocateMemory = NULL;
PFN_vkBindImageMemory vkBindImageMemory = NULL;
PFN_vkCreateRenderPass vkCreateRenderPass = NULL;
PFN_vkCreateFramebuffer vkCreateFramebuffer = NULL;
PFN_vkCreateBuffer vkCreateBuffer = NULL;
PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements = NULL;
PFN_vkMapMemory vkMapMemory = NULL;
PFN_vkUnmapMemory vkUnmapMemory = NULL;
PFN_vkBindBufferMemory vkBindBufferMemory = NULL;
PFN_vkCreateShaderModule vkCreateShaderModule = NULL;
PFN_vkCreatePipelineLayout vkCreatePipelineLayout = NULL;
PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines = NULL;
PFN_vkCreateSemaphore vkCreateSemaphore = NULL;
PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass = NULL;
PFN_vkCmdBindPipeline vkCmdBindPipeline = NULL;
PFN_vkCmdSetViewport vkCmdSetViewport = NULL;
PFN_vkCmdSetScissor vkCmdSetScissor = NULL;
PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers = NULL;
PFN_vkCmdDraw vkCmdDraw = NULL;
PFN_vkCmdEndRenderPass vkCmdEndRenderPass = NULL;
PFN_vkDestroyFence vkDestroyFence = NULL;
PFN_vkDestroySemaphore vkDestroySemaphore = NULL;
PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout = NULL;
PFN_vkCreateDescriptorPool vkCreateDescriptorPool = NULL;
PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets = NULL;
PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets = NULL;
PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets = NULL;
PFN_vkFlushMappedMemoryRanges vkFlushMappedMemoryRanges = NULL;
PFN_vkCreateSampler vkCreateSampler = NULL;

// Ext
PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = NULL;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = NULL;
PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = NULL;

PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = NULL;
PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = NULL;
PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = NULL;
PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = NULL;
PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = NULL;

PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR = NULL;
PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR = NULL;
PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR = NULL;
PFN_vkQueuePresentKHR vkQueuePresentKHR = NULL;

void win32_LoadVulkan() {
    HMODULE vulkanModule = LoadLibrary(convertToLPCWSTR("vulkan-1.dll"));
    assert(vulkanModule, "Failed to load Vulkan module.");

    vkCreateInstance = (PFN_vkCreateInstance)GetProcAddress(vulkanModule, "vkCreateInstance");
    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)GetProcAddress(vulkanModule, "vkEnumerateInstanceLayerProperties");
    vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)GetProcAddress(vulkanModule, "vkEnumerateInstanceExtensionProperties");
    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkanModule, "vkGetInstanceProcAddr");
    vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)GetProcAddress(vulkanModule, "vkEnumeratePhysicalDevices");
    vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)GetProcAddress(vulkanModule, "vkGetPhysicalDeviceProperties");
    vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)GetProcAddress(vulkanModule, "vkGetPhysicalDeviceQueueFamilyProperties");
    vkCreateDevice = (PFN_vkCreateDevice)GetProcAddress(vulkanModule, "vkCreateDevice");
    vkGetDeviceQueue = (PFN_vkGetDeviceQueue)GetProcAddress(vulkanModule, "vkGetDeviceQueue");
    vkCreateCommandPool = (PFN_vkCreateCommandPool)GetProcAddress(vulkanModule, "vkCreateCommandPool");
    vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)GetProcAddress(vulkanModule, "vkAllocateCommandBuffers");
    vkCreateFence = (PFN_vkCreateFence)GetProcAddress(vulkanModule, "vkCreateFence");
    vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)GetProcAddress(vulkanModule, "vkBeginCommandBuffer");
    vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)GetProcAddress(vulkanModule, "vkCmdPipelineBarrier");
    vkEndCommandBuffer = (PFN_vkEndCommandBuffer)GetProcAddress(vulkanModule, "vkEndCommandBuffer");
    vkQueueSubmit = (PFN_vkQueueSubmit)GetProcAddress(vulkanModule, "vkQueueSubmit");
    vkWaitForFences = (PFN_vkWaitForFences)GetProcAddress(vulkanModule, "vkWaitForFences");
    vkResetFences = (PFN_vkResetFences)GetProcAddress(vulkanModule, "vkResetFences");
    vkResetCommandBuffer = (PFN_vkResetCommandBuffer)GetProcAddress(vulkanModule, "vkResetCommandBuffer");
    vkCreateImageView = (PFN_vkCreateImageView)GetProcAddress(vulkanModule, "vkCreateImageView");
    vkCreateImage = (PFN_vkCreateImage)GetProcAddress(vulkanModule, "vkCreateImage");
    vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)GetProcAddress(vulkanModule, "vkGetImageMemoryRequirements");
    vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)GetProcAddress(vulkanModule, "vkGetPhysicalDeviceMemoryProperties");
    vkAllocateMemory = (PFN_vkAllocateMemory)GetProcAddress(vulkanModule, "vkAllocateMemory");
    vkBindImageMemory = (PFN_vkBindImageMemory)GetProcAddress(vulkanModule, "vkBindImageMemory");
    vkCreateRenderPass = (PFN_vkCreateRenderPass)GetProcAddress(vulkanModule, "vkCreateRenderPass");
    vkCreateFramebuffer = (PFN_vkCreateFramebuffer)GetProcAddress(vulkanModule, "vkCreateFramebuffer");
    vkCreateBuffer = (PFN_vkCreateBuffer)GetProcAddress(vulkanModule, "vkCreateBuffer");
    vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)GetProcAddress(vulkanModule, "vkGetBufferMemoryRequirements");
    vkMapMemory = (PFN_vkMapMemory)GetProcAddress(vulkanModule, "vkMapMemory");
    vkUnmapMemory = (PFN_vkUnmapMemory)GetProcAddress(vulkanModule, "vkUnmapMemory");
    vkBindBufferMemory = (PFN_vkBindBufferMemory)GetProcAddress(vulkanModule, "vkBindBufferMemory");
    vkCreateShaderModule = (PFN_vkCreateShaderModule)GetProcAddress(vulkanModule, "vkCreateShaderModule");
    vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)GetProcAddress(vulkanModule, "vkCreatePipelineLayout");
    vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)GetProcAddress(vulkanModule, "vkCreateGraphicsPipelines");
    vkCreateSemaphore = (PFN_vkCreateSemaphore)GetProcAddress(vulkanModule, "vkCreateSemaphore");
    vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)GetProcAddress(vulkanModule, "vkCmdBeginRenderPass");
    vkCmdBindPipeline = (PFN_vkCmdBindPipeline)GetProcAddress(vulkanModule, "vkCmdBindPipeline");
    vkCmdSetViewport = (PFN_vkCmdSetViewport)GetProcAddress(vulkanModule, "vkCmdSetViewport");
    vkCmdSetScissor = (PFN_vkCmdSetScissor)GetProcAddress(vulkanModule, "vkCmdSetScissor");
    vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)GetProcAddress(vulkanModule, "vkCmdBindVertexBuffers");
    vkCmdDraw = (PFN_vkCmdDraw)GetProcAddress(vulkanModule, "vkCmdDraw");
    vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)GetProcAddress(vulkanModule, "vkCmdEndRenderPass");
    vkDestroyFence = (PFN_vkDestroyFence)GetProcAddress(vulkanModule, "vkDestroyFence");
    vkDestroySemaphore = (PFN_vkDestroySemaphore)GetProcAddress(vulkanModule, "vkDestroySemaphore");
    vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)GetProcAddress(vulkanModule, "vkCreateDescriptorSetLayout");
    vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)GetProcAddress(vulkanModule, "vkCreateDescriptorPool");
    vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)GetProcAddress(vulkanModule, "vkAllocateDescriptorSets");
    vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)GetProcAddress(vulkanModule, "vkUpdateDescriptorSets");
    vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)GetProcAddress(vulkanModule, "vkCmdBindDescriptorSets");
    vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)GetProcAddress(vulkanModule, "vkFlushMappedMemoryRanges");
    vkCreateSampler = (PFN_vkCreateSampler)GetProcAddress(vulkanModule, "vkCreateSampler");
}

void win32_LoadVulkanExtensions(vulkan_context &context) {
    *(void **)&vkCreateDebugReportCallbackEXT = vkGetInstanceProcAddr(context.instance, "vkCreateDebugReportCallbackEXT");
    *(void **)&vkDestroyDebugReportCallbackEXT = vkGetInstanceProcAddr(context.instance, "vkDestroyDebugReportCallbackEXT");
    *(void **)&vkDebugReportMessageEXT = vkGetInstanceProcAddr(context.instance, "vkDebugReportMessageEXT");

    *(void **)&vkCreateWin32SurfaceKHR = vkGetInstanceProcAddr(context.instance, "vkCreateWin32SurfaceKHR");
    *(void **)&vkGetPhysicalDeviceSurfaceSupportKHR = vkGetInstanceProcAddr(context.instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
    *(void **)&vkGetPhysicalDeviceSurfaceFormatsKHR = vkGetInstanceProcAddr(context.instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    *(void **)&vkGetPhysicalDeviceSurfaceCapabilitiesKHR = vkGetInstanceProcAddr(context.instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
    *(void **)&vkGetPhysicalDeviceSurfacePresentModesKHR = vkGetInstanceProcAddr(context.instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");

    *(void **)&vkCreateSwapchainKHR = vkGetInstanceProcAddr(context.instance, "vkCreateSwapchainKHR");
    *(void **)&vkGetSwapchainImagesKHR = vkGetInstanceProcAddr(context.instance, "vkGetSwapchainImagesKHR");
    *(void **)&vkAcquireNextImageKHR = vkGetInstanceProcAddr(context.instance, "vkAcquireNextImageKHR");
    *(void **)&vkQueuePresentKHR = vkGetInstanceProcAddr(context.instance, "vkQueuePresentKHR");
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
    uint64_t object, size_t location, int32_t messageCode, const char *pLayerPrefix, const char *pMessage, void *pUserData) {

    OutputDebugStringA(pLayerPrefix);
    OutputDebugStringA(" ");
    OutputDebugStringA(pMessage);
    OutputDebugStringA("\n");
    return VK_FALSE;
}

void render() {
    VkSemaphore presentCompleteSem, renderingCompleteSem;
    VkSemaphoreCreateInfo semaphoreCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, 0, 0 };
    vkCreateSemaphore(context.device, &semaphoreCreateInfo, NULL, &presentCompleteSem);
    vkCreateSemaphore(context.device, &semaphoreCreateInfo, NULL, &renderingCompleteSem);

    uint32_t nextImgIdx;
    vkAcquireNextImageKHR(context.device, context.swapchain, UINT64_MAX, presentCompleteSem, VK_NULL_HANDLE, &nextImgIdx);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(context.drawCmdBuffer, &beginInfo);

    VkImageMemoryBarrier layoutTransitionBarrier = {};
    layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    layoutTransitionBarrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    layoutTransitionBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    layoutTransitionBarrier.image = context.presentImages[nextImgIdx];
    layoutTransitionBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    vkCmdPipelineBarrier(
        context.drawCmdBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &layoutTransitionBarrier);

    VkClearValue clearValue[] = { { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } };
    VkRenderPassBeginInfo renderpassBeginInfo = {};
    renderpassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpassBeginInfo.renderPass = context.renderpass;
    renderpassBeginInfo.framebuffer = context.frameBuffers[nextImgIdx];
    VkRect2D renderArea = { 0, 0, context.width, context.height };
    renderpassBeginInfo.renderArea = renderArea;
    renderpassBeginInfo.clearValueCount = 2;
    renderpassBeginInfo.pClearValues = clearValue;
    vkCmdBeginRenderPass(context.drawCmdBuffer, &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(context.drawCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, context.pipeline);

    VkViewport viewport = { 0, 0, context.width, context.height, 0, 1 };
    vkCmdSetViewport(context.drawCmdBuffer, 0, 1, &viewport);

    VkRect2D scissor = { 0, 0, context.width, context.height };
    vkCmdSetScissor(context.drawCmdBuffer, 0, 1, &scissor);

    VkDeviceSize offsets = {};
    vkCmdBindVertexBuffers(context.drawCmdBuffer, 0, 1, &context.vertexInputBuffer, &offsets);

    vkCmdDraw(context.drawCmdBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(context.drawCmdBuffer);

    VkImageMemoryBarrier prePresentBarrier = {};
    prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    prePresentBarrier.image = context.presentImages[nextImgIdx];
    prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    
    vkCmdPipelineBarrier(
        context.drawCmdBuffer,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0,
        0,
        NULL,
        0,
        NULL,
        1,
        &prePresentBarrier);

    vkEndCommandBuffer(context.drawCmdBuffer);

    // Present
    VkFence renderFence;
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCreateFence(context.device, &fenceCreateInfo, NULL, &renderFence);

    VkPipelineStageFlags waitStageMask = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &presentCompleteSem;
    submitInfo.pWaitDstStageMask = &waitStageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &context.drawCmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderingCompleteSem;

    vkQueueSubmit(context.presentQueue, 1, &submitInfo, renderFence);

    vkWaitForFences(context.device, 1, &renderFence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(context.device, renderFence, NULL);

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderingCompleteSem;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &context.swapchain;
    presentInfo.pImageIndices = &nextImgIdx;
    presentInfo.pResults = NULL;

    vkQueuePresentKHR(context.presentQueue, &presentInfo);

    vkDestroySemaphore(context.device, presentCompleteSem, NULL);
    vkDestroySemaphore(context.device, renderingCompleteSem, NULL);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)  {
    switch (uMsg) {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_PAINT:
        render();
        break;
    default:
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = convertToLPCWSTR("VulkanWindowClass");
    RegisterClassEx(&windowClass);

    context.width = 800;
    context.height = 600;

    HWND windowHandle = CreateWindowEx(
        NULL,
        convertToLPCWSTR("VulkanWindowClass"),
        convertToLPCWSTR("Vulkan Triangle"),
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        100,
        100,
        context.width,
        context.height,
        NULL,
        NULL,
        hInstance,
        NULL);

    win32_LoadVulkan();

    VkResult res;

    // Validation Layers
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    assert(layerCount != 0, "Failed to find any layers in the system.");

    VkLayerProperties *layersAvailable = new VkLayerProperties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, layersAvailable);

    bool foundValidationLayer = false;
    for (int i = 0; i < layerCount; ++i) {
        if (strcmp(layersAvailable[i].layerName, "VK_LAYER_LUNARG_standard_validation") == 0) {
            foundValidationLayer = true;
        }
    }
    delete[] layersAvailable;

    assert(foundValidationLayer, "Failed to find validation layer.");
    const char *layers[] = { "VK_LAYER_LUNARG_standard_validation" };

    // Extensions
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, NULL);
    VkExtensionProperties *extensionsAvailable = new VkExtensionProperties[extensionCount];
    vkEnumerateInstanceExtensionProperties(NULL, &extensionCount, extensionsAvailable);

    const char *extensions[] = {
        "VK_KHR_surface",
        "VK_KHR_win32_surface",
        "VK_EXT_debug_report"
    };

    uint32_t foundExtensions = 0;
    for (int i = 0; i < extensionCount; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (strcmp(extensionsAvailable[i].extensionName, extensions[j]) == 0) {
                ++foundExtensions;
            }
        }
    }
    delete[] extensionsAvailable;

    assert(foundExtensions == 3, "Couldn't find expected extensions.");

    VkApplicationInfo applicationInfo = {};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Vulkan Triangle";
    applicationInfo.engineVersion = 1;
    applicationInfo.apiVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &applicationInfo;
    instanceInfo.enabledLayerCount = 1;
    instanceInfo.ppEnabledLayerNames = layers;
    instanceInfo.enabledExtensionCount = 3;
    instanceInfo.ppEnabledExtensionNames = extensions;

    res = vkCreateInstance(&instanceInfo, NULL, &context.instance);
    checkVkResult(res, "Failed to create Vulkan instance.");

    win32_LoadVulkanExtensions(context);

    // Setup Debug Callback
    VkDebugReportCallbackCreateInfoEXT callbackInfo = {};
    callbackInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    callbackInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    callbackInfo.pfnCallback = &DebugReportCallback;
    callbackInfo.pUserData = NULL;

    res = vkCreateDebugReportCallbackEXT(context.instance, &callbackInfo, NULL, &context.debugCb);
    checkVkResult(res, "Failed to create debug report callback.");

    // Create windows surface
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.hinstance = hInstance;
    surfaceInfo.hwnd = windowHandle;

    res = vkCreateWin32SurfaceKHR(context.instance, &surfaceInfo, NULL, &context.surface);
    checkVkResult(res, "Failed to create surface.");

    // Find pysical device with a graphics queue
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, NULL);
    VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
    vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, physicalDevices);

    for (int i = 0; i < physicalDeviceCount; ++i) {
        VkPhysicalDeviceProperties deviceProperties = {};
        vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, NULL);
        VkQueueFamilyProperties *queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyCount];
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyCount, queueFamilyProperties);

        for (int j = 0; j < queueFamilyCount; ++j) {
            VkBool32 supportsPreset;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[i], j, context.surface, &supportsPreset);

            if (supportsPreset && queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                context.physicalDevice = physicalDevices[i];
                context.physicalDeviceProperties = deviceProperties;
                context.presentQueueIndex = j;
                break;
            }
        }

        delete[] queueFamilyProperties;
        if (context.physicalDevice) {
            break;
        }
    }
    delete[] physicalDevices;

    assert(context.physicalDevice, "Failed to find a physical device that can render and present.");

    vkGetPhysicalDeviceMemoryProperties(context.physicalDevice, &context.memoryProperties);

    // Create logical device
    VkDeviceQueueCreateInfo deviceQueueInfo = {};
    deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueInfo.queueFamilyIndex = context.presentQueueIndex;
    deviceQueueInfo.queueCount = 1;
    float queuePriorities[] = { 1.0f };
    deviceQueueInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo deviceInfo = {};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &deviceQueueInfo;
    deviceInfo.enabledLayerCount = 1;
    deviceInfo.ppEnabledLayerNames = layers;

    const char *deviceExtensions[] = { "VK_KHR_swapchain" };
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;

    VkPhysicalDeviceFeatures features = {};
    features.shaderClipDistance = VK_TRUE;
    deviceInfo.pEnabledFeatures = &features;

    res = vkCreateDevice(context.physicalDevice, &deviceInfo, NULL, &context.device);
    checkVkResult(res, "Failed to create logical device.");

    vkGetDeviceQueue(context.device, context.presentQueueIndex, 0, &context.presentQueue);

    // Create command buffers
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = context.presentQueueIndex;

    VkCommandPool cmdPool;
    res = vkCreateCommandPool(context.device, &cmdPoolInfo, NULL, &cmdPool);
    checkVkResult(res, "Failed to create command pool.");

    VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
    cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocInfo.commandPool = cmdPool;
    cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferAllocInfo.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(context.device, &cmdBufferAllocInfo, &context.setupCmdBuffer);
    checkVkResult(res, "Failed to allocate setup command buffer.");

    res = vkAllocateCommandBuffers(context.device, &cmdBufferAllocInfo, &context.drawCmdBuffer);
    checkVkResult(res, "Failed to allocate draw command buffer.");

    // Create swapchain
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCount, NULL);
    VkSurfaceFormatKHR *surfaceFormats = new VkSurfaceFormatKHR[formatCount];
    vkGetPhysicalDeviceSurfaceFormatsKHR(context.physicalDevice, context.surface, &formatCount, surfaceFormats);

    VkFormat colorFormat;
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
        colorFormat = VK_FORMAT_B8G8R8_UNORM;
    } else {
        colorFormat = surfaceFormats[0].format;
    }

    VkColorSpaceKHR colorSpace;
    colorSpace = surfaceFormats[0].colorSpace;
    delete[] surfaceFormats;

    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.physicalDevice, context.surface, &surfaceCapabilities);

    uint32_t desiredImageCount = 2;
    if (desiredImageCount < surfaceCapabilities.minImageCount) {
        desiredImageCount = surfaceCapabilities.minImageCount;
    } else if (surfaceCapabilities.maxImageCount != 0 && desiredImageCount > surfaceCapabilities.maxImageCount) {
        desiredImageCount = surfaceCapabilities.maxImageCount;
    }

    VkExtent2D surfaceResolution = surfaceCapabilities.currentExtent;
    if (surfaceResolution.width == -1) {
        surfaceResolution.width = context.width;
        surfaceResolution.height = context.height;
    } else {
        context.width = surfaceResolution.width;
        context.height = surfaceResolution.height;
    }

    VkSurfaceTransformFlagBitsKHR preTransform = surfaceCapabilities.currentTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentModeCount, NULL);
    VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentModeCount];
    vkGetPhysicalDeviceSurfacePresentModesKHR(context.physicalDevice, context.surface, &presentModeCount, presentModes);

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (int i = 0; i < presentModeCount; ++i) {
        if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }
    delete[] presentModes;

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = context.surface;
    swapchainInfo.minImageCount = desiredImageCount;
    swapchainInfo.imageFormat = colorFormat;
    swapchainInfo.imageColorSpace = colorSpace;
    swapchainInfo.imageExtent = surfaceResolution;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.preTransform = preTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = true;
    swapchainInfo.oldSwapchain = NULL;

    res = vkCreateSwapchainKHR(context.device, &swapchainInfo, NULL, &context.swapchain);
    checkVkResult(res, "Failed to create swapchain.");

    // Create image views
    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(context.device, context.swapchain, &imageCount, NULL);
    context.presentImages = new VkImage[imageCount];
    vkGetSwapchainImagesKHR(context.device, context.swapchain, &imageCount, context.presentImages);

    VkImageViewCreateInfo presentImageViewInfo = {};
    presentImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    presentImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    presentImageViewInfo.format = colorFormat;
    presentImageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
    presentImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    presentImageViewInfo.subresourceRange.baseMipLevel = 0;
    presentImageViewInfo.subresourceRange.levelCount = 1;
    presentImageViewInfo.subresourceRange.baseArrayLayer = 0;
    presentImageViewInfo.subresourceRange.layerCount = 1;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence submitFence;
    vkCreateFence(context.device, &fenceCreateInfo, NULL, &submitFence);

    bool *transitioned = new bool[imageCount];
    memset(transitioned, 0, sizeof(bool) * imageCount);
    uint32_t doneCount = 0;
    while (doneCount != imageCount) {
        VkSemaphore presentCompleteSem;
        VkSemaphoreCreateInfo semCreateInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, 0, 0 };
        vkCreateSemaphore(context.device, &semCreateInfo, NULL, &presentCompleteSem);

        uint32_t nextImgIdx;
        vkAcquireNextImageKHR(context.device, context.swapchain, UINT64_MAX, presentCompleteSem, VK_NULL_HANDLE, &nextImgIdx);
        if (!transitioned[nextImgIdx]) {
            vkBeginCommandBuffer(context.setupCmdBuffer, &beginInfo);

            VkImageMemoryBarrier layoutTransitionBarrier = {};
            layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            layoutTransitionBarrier.srcAccessMask = 0;
            layoutTransitionBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            layoutTransitionBarrier.image = context.presentImages[nextImgIdx];

            VkImageSubresourceRange resourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
            layoutTransitionBarrier.subresourceRange = resourceRange;

            vkCmdPipelineBarrier(
                context.setupCmdBuffer,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                0,
                0,
                NULL,
                0,
                NULL,
                1,
                &layoutTransitionBarrier);

            vkEndCommandBuffer(context.setupCmdBuffer);

            VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &presentCompleteSem;
            submitInfo.pWaitDstStageMask = waitStageMask;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &context.setupCmdBuffer;
            submitInfo.signalSemaphoreCount = 0;
            submitInfo.pSignalSemaphores = NULL;
            res = vkQueueSubmit(context.presentQueue, 1, &submitInfo, submitFence);

            vkWaitForFences(context.device, 1, &submitFence, VK_TRUE, UINT64_MAX);
            vkResetFences(context.device, 1, &submitFence);

            vkDestroySemaphore(context.device, presentCompleteSem, NULL);

            vkResetCommandBuffer(context.setupCmdBuffer, 0);

            transitioned[nextImgIdx] = true;
            ++doneCount;
        }

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 0;
        presentInfo.pWaitSemaphores = NULL;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &context.swapchain;
        presentInfo.pImageIndices = &nextImgIdx;
        vkQueuePresentKHR(context.presentQueue, &presentInfo);
    }
    delete[] transitioned;

    VkImageView *presentImageViews = new VkImageView[imageCount];
    for (int i = 0; i < imageCount; ++i) {
        presentImageViewInfo.image = context.presentImages[i];

        res = vkCreateImageView(context.device, &presentImageViewInfo, NULL, &presentImageViews[i]);
        checkVkResult(res, "Failed to create image view.");
    }

    // Create depth image
    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = VK_FORMAT_D16_UNORM;
    imageCreateInfo.extent = { context.width, context.height, 1 };
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = NULL;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    res = vkCreateImage(context.device, &imageCreateInfo, NULL, &context.depthImage);
    checkVkResult(res, "Failed to create depth image.");

    VkMemoryRequirements memoryRequirements = {};
    vkGetImageMemoryRequirements(context.device, context.depthImage, &memoryRequirements);

    VkMemoryAllocateInfo imageAllocateInfo = {};
    imageAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    imageAllocateInfo.allocationSize = memoryRequirements.size;

    uint32_t memoryTypeBits = memoryRequirements.memoryTypeBits;
    VkMemoryPropertyFlags desiredMemFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    for (int i = 0; i < 32; ++i) {
        VkMemoryType memoryType = context.memoryProperties.memoryTypes[i];
        if (memoryTypeBits & 1) {
            if (desiredMemFlags == (memoryType.propertyFlags & desiredMemFlags)) {
                imageAllocateInfo.memoryTypeIndex = i;
                break;
            }
        }
        memoryTypeBits = memoryTypeBits >> 1;
    }

    VkDeviceMemory imageMemory = {};
    res = vkAllocateMemory(context.device, &imageAllocateInfo, NULL, &imageMemory);
    checkVkResult(res, "Failed to allocate device memory.");

    res = vkBindImageMemory(context.device, context.depthImage, imageMemory, 0);
    checkVkResult(res, "Failed to bind image memory.");

    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(context.setupCmdBuffer, &beginInfo);

        VkImageMemoryBarrier layoutTransitionBarrier = {};
        layoutTransitionBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        layoutTransitionBarrier.srcAccessMask = 0;
        layoutTransitionBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        layoutTransitionBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        layoutTransitionBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        layoutTransitionBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        layoutTransitionBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        layoutTransitionBarrier.image = context.depthImage;

        VkImageSubresourceRange resourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
        layoutTransitionBarrier.subresourceRange = resourceRange;

        vkCmdPipelineBarrier(
            context.setupCmdBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            0,
            0,
            NULL,
            0,
            NULL,
            1,
            &layoutTransitionBarrier);

        vkEndCommandBuffer(context.setupCmdBuffer);

        VkPipelineStageFlags waitStageMask[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = NULL;
        submitInfo.pWaitDstStageMask = waitStageMask;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &context.setupCmdBuffer;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = NULL;
        res = vkQueueSubmit(context.presentQueue, 1, &submitInfo, submitFence);

        vkWaitForFences(context.device, 1, &submitFence, VK_TRUE, UINT64_MAX);
        vkResetFences(context.device, 1, &submitFence);
        vkResetCommandBuffer(context.setupCmdBuffer, 0);
    }

    VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = context.depthImage;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = imageCreateInfo.format;
    imageViewCreateInfo.components = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
    imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    res = vkCreateImageView(context.device, &imageViewCreateInfo, NULL, &context.depthImageView);
    checkVkResult(res, "Failed to create depth image view.");

    // Create Framebuffers
    VkAttachmentDescription passAttachments[2] = {};
    passAttachments[0].format = colorFormat;
    passAttachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    passAttachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    passAttachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    passAttachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    passAttachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    passAttachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    passAttachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    passAttachments[1].format = VK_FORMAT_D16_UNORM;
    passAttachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    passAttachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    passAttachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    passAttachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    passAttachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    passAttachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    passAttachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Subpass
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentReference;
    subpass.pDepthStencilAttachment = &depthAttachmentReference;

    // Renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 2;
    renderPassInfo.pAttachments = passAttachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    res = vkCreateRenderPass(context.device, &renderPassInfo, NULL, &context.renderpass);
    checkVkResult(res, "Failed to create renderpass.");

    VkImageView frameBufferAttachments[2];
    frameBufferAttachments[1] = context.depthImageView;

    VkFramebufferCreateInfo frameBufferCreateInfo = {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = context.renderpass;
    frameBufferCreateInfo.attachmentCount = 2;
    frameBufferCreateInfo.pAttachments = frameBufferAttachments;
    frameBufferCreateInfo.width = context.width;
    frameBufferCreateInfo.height = context.height;
    frameBufferCreateInfo.layers = 1;

    context.frameBuffers = new VkFramebuffer[imageCount];
    for (int i = 0; i < imageCount; ++i) {
        frameBufferAttachments[0] = presentImageViews[i];
        res = vkCreateFramebuffer(context.device, &frameBufferCreateInfo, NULL, &context.frameBuffers[i]);
        checkVkResult(res, "Failed to create framebuffer.");
    }

    // Vertex Buffer
    struct vertex {
        float x, y, z, w;
    };

    VkBufferCreateInfo vertexInputBufferInfo = {};
    vertexInputBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vertexInputBufferInfo.size = 3 * sizeof(vertex);
    vertexInputBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vertexInputBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vertexInputBufferInfo.queueFamilyIndexCount = 0;
    vertexInputBufferInfo.pQueueFamilyIndices = NULL;

    res = vkCreateBuffer(context.device, &vertexInputBufferInfo, NULL, &context.vertexInputBuffer);
    checkVkResult(res, "Failed to create vertex input buffer.");

    VkMemoryRequirements vertexBufferMemoryRequirements = {};
    vkGetBufferMemoryRequirements(context.device, context.vertexInputBuffer, &vertexBufferMemoryRequirements);

    VkMemoryAllocateInfo vertexBufferAllocateInfo = {};
    vertexBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vertexBufferAllocateInfo.pNext = NULL;
    vertexBufferAllocateInfo.allocationSize = vertexBufferMemoryRequirements.size;

    uint32_t vertexMemoryTypeBits = vertexBufferMemoryRequirements.memoryTypeBits;
    VkMemoryPropertyFlags vertexDesiredMemoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    for (int i = 0; i < 32; ++i) {
        VkMemoryType memoryType = context.memoryProperties.memoryTypes[i];
        if (vertexMemoryTypeBits & 1) {
            if (vertexDesiredMemoryFlags == (memoryType.propertyFlags & vertexDesiredMemoryFlags)) {
                vertexBufferAllocateInfo.memoryTypeIndex = i;
                break;
            }
        }
        vertexMemoryTypeBits = vertexMemoryTypeBits >> 1;
    }

    VkDeviceMemory vertexBufferMemory;
    res = vkAllocateMemory(context.device, &vertexBufferAllocateInfo, NULL, &vertexBufferMemory);
    checkVkResult(res, "Failed to allocate vertex buffer memory.");

    void *mapped;
    res = vkMapMemory(context.device, vertexBufferMemory, 0, VK_WHOLE_SIZE, 0, &mapped);
    checkVkResult(res, "Failed to map vertex buffer memory.");

    vertex *triangle = (vertex *)mapped;
    vertex v1 = { -1.0f,  1.0f, 0.0f, 1.0f };
    vertex v2 = {  1.0f,  1.0f, 0.0f, 1.0f };
    vertex v3 = {  0.0f, -1.0f, 0.0f, 1.0f };
    triangle[0] = v1;
    triangle[1] = v2;
    triangle[2] = v3;

    vkUnmapMemory(context.device, vertexBufferMemory);
    res = vkBindBufferMemory(context.device, context.vertexInputBuffer, vertexBufferMemory, 0);
    checkVkResult(res, "Failed to bind vertex buffer memory.");

    // Shaders
    uint32_t codeSize;
    char *code = new char[10000];
    HANDLE fileHandle = 0;

    // VS
    fileHandle = CreateFile(convertToLPCWSTR("vert.spv"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("Failed to open vertex shader file.\n");
        exit(1);
    }
    ReadFile(fileHandle, code, 10000, (LPDWORD)&codeSize, 0);
    CloseHandle(fileHandle);

    VkShaderModuleCreateInfo vertexShaderInfo = {};
    vertexShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vertexShaderInfo.codeSize = codeSize;
    vertexShaderInfo.pCode = (uint32_t *)code;

    VkShaderModule vertexShaderModule;
    res = vkCreateShaderModule(context.device, &vertexShaderInfo, NULL, &vertexShaderModule);
    checkVkResult(res, "Faileld to create vertex shader module.");

    // FS
    fileHandle = CreateFile(convertToLPCWSTR("frag.spv"), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        OutputDebugStringA("Failed to open fragment shader file.\n");
        exit(1);
    }
    ReadFile(fileHandle, code, 10000, (LPDWORD)&codeSize, 0);
    CloseHandle(fileHandle);

    VkShaderModuleCreateInfo fragmentShaderInfo = {};
    fragmentShaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fragmentShaderInfo.codeSize = codeSize;
    fragmentShaderInfo.pCode = (uint32_t *)code;

    VkShaderModule fragmentShaderModule;
    res = vkCreateShaderModule(context.device, &fragmentShaderInfo, NULL, &fragmentShaderModule);
    checkVkResult(res, "Failed to create fragment shader module.");

    // Graphics Pipeline
    VkPipelineLayoutCreateInfo layoutCreateInfo = {};
    layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutCreateInfo.setLayoutCount = 0;
    layoutCreateInfo.pSetLayouts = NULL;
    layoutCreateInfo.pushConstantRangeCount = 0;
    layoutCreateInfo.pPushConstantRanges = NULL;

    res = vkCreatePipelineLayout(context.device, &layoutCreateInfo, NULL, &context.pipelineLayout);
    checkVkResult(res, "Failed to create pipeline layout.");

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {};
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module = vertexShaderModule;
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[0].pSpecializationInfo = NULL;

    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module = fragmentShaderModule;
    shaderStageCreateInfo[1].pName = "main";
    shaderStageCreateInfo[1].pSpecializationInfo = NULL;

    VkVertexInputBindingDescription vertexBindingDescription = {};
    vertexBindingDescription.binding = 0;
    vertexBindingDescription.stride = sizeof(vertex);
    vertexBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertexAttributeDescription = {};
    vertexAttributeDescription.location = 0;
    vertexAttributeDescription.binding = 0;
    vertexAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexAttributeDescription.offset = 0;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = &vertexAttributeDescription;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    // Viewport
    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = 0;
    viewport.width = context.width;
    viewport.height = context.height;
    viewport.minDepth = 0;
    viewport.maxDepth = 1;

    VkRect2D scissors = {};
    scissors.offset = { 0, 0 };
    scissors.extent = { context.width, context.height };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissors;

    // Rasterization
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasConstantFactor = 0;
    rasterizationStateCreateInfo.depthBiasClamp = 0;
    rasterizationStateCreateInfo.depthBiasSlopeFactor = 0;
    rasterizationStateCreateInfo.lineWidth = 1;

    // Sampling
    VkPipelineMultisampleStateCreateInfo sampleStateCreateInfo = {};
    sampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    sampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    sampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    sampleStateCreateInfo.minSampleShading = 0;
    sampleStateCreateInfo.pSampleMask = NULL;
    sampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    sampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    // Depth-Stencil
    VkStencilOpState stencilState = {};
    stencilState.failOp = VK_STENCIL_OP_KEEP;
    stencilState.passOp = VK_STENCIL_OP_KEEP;
    stencilState.depthFailOp = VK_STENCIL_OP_KEEP;
    stencilState.compareOp = VK_COMPARE_OP_ALWAYS;
    stencilState.compareMask = 0;
    stencilState.writeMask = 0;
    stencilState.reference = 0;

    VkPipelineDepthStencilStateCreateInfo dsStateCreateInfo = {};
    dsStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    dsStateCreateInfo.depthTestEnable = VK_TRUE;
    dsStateCreateInfo.depthWriteEnable = VK_TRUE;
    dsStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    dsStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    dsStateCreateInfo.stencilTestEnable = VK_FALSE;
    dsStateCreateInfo.front = stencilState;
    dsStateCreateInfo.back = stencilState;
    dsStateCreateInfo.minDepthBounds = 0;
    dsStateCreateInfo.maxDepthBounds = 0;

    // Color blend
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
    colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState.colorWriteMask = 0xf;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_CLEAR;
    colorBlendState.attachmentCount = 1;
    colorBlendState.pAttachments = &colorBlendAttachmentState;
    colorBlendState.blendConstants[0] = 0.0;
    colorBlendState.blendConstants[1] = 0.0;
    colorBlendState.blendConstants[2] = 0.0;
    colorBlendState.blendConstants[3] = 0.0;

    // Dynamic state
    VkDynamicState dynamicState[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicState;

    // Pipeline
    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStageCreateInfo;
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pTessellationState = NULL;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &sampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &dsStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = context.pipelineLayout;
    pipelineCreateInfo.renderPass = context.renderpass;
    pipelineCreateInfo.subpass = 0;
    pipelineCreateInfo.basePipelineHandle = NULL;
    pipelineCreateInfo.basePipelineIndex = 0;

    res = vkCreateGraphicsPipelines(context.device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, NULL, &context.pipeline);
    checkVkResult(res, "Failed to create graphics pipeline.");


    MSG msg;
    bool done = false;
    while (!done) {
        PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE);
        if (msg.message == WM_QUIT) {
            done = true;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        RedrawWindow(windowHandle, NULL, NULL, RDW_INTERNALPAINT);
    }

    vkDestroyDebugReportCallbackEXT(context.instance, context.debugCb, NULL);

    return msg.wParam;
}