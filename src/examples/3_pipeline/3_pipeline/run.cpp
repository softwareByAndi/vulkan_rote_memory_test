#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include "lib/snippets/useful_functions.h"
#include "lib/snippets/io_macros.h"
#include "lib/snippets/vulkan_io.h"

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
std::vector<const char *> instanceEXT {
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
std::vector<const char *> instanceLAY {
  // validation
};
std::vector<const char *> deviceEXT {
  "VK_KHR_portability_subset",
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkDevice device;
VkQueue queue;
VkPhysicalDevice physicalDevice;
VkPhysicalDeviceFeatures pdFeatures;
float queuePriority = 1.0f;
uint32_t queueFamilyIndex = 0;

GLFWwindow * window = nullptr;
VkSurfaceKHR surface;
VkSurfaceCapabilitiesKHR surfCaps;

VkSwapchainKHR swapchain;
VkFormat swapFormat = VK_FORMAT_B8G8R8A8_SRGB;
VkColorSpaceKHR swapColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
VkExtent2D swapExtent{};
std::vector<VkImage> swapImages{};
std::vector<VkImageView> swapImageViews{};
VkShaderModule vertShaderModule;
VkShaderModule fragShaderModule;

VkRenderPass renderPass;
VkSampleCountFlagBits renderSampleCount = VK_SAMPLE_COUNT_1_BIT;

VkPipelineLayout pipelineLayout;
VkPipeline pipeline;


std::vector<char> readFile(const std::string& fileName) {
  std::ifstream file(fileName, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    FAIL("failed to open file");
  }
  long fileSize = file.tellg();
  std::vector<char> buffer(fileSize);
  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();
  return buffer;
}
VkShaderModule createShaderModule(const std::vector<char>& code) {
  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = code.size();
  info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shader;
  if (VK_SUCCESS != vkCreateShaderModule(device, &info, nullptr, &shader)) {
    FAIL("failed to create shader");
  }
  return shader;
}

int main() {
  SECTION ("GLFW") {
    glfwInit();
    uint32_t count = 0;
    auto glfwEXT = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = 0; i < count; i++) {
      instanceEXT.push_back(glfwEXT[i]);
    }
  }
  SECTION("INSTANCE") {
    VkApplicationInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pApplicationName = "3";
    ai.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.pEngineName = "ASDF";
    ai.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    info.pApplicationInfo = &ai;
    info.enabledLayerCount = instanceLAY.size();
    info.ppEnabledLayerNames = instanceLAY.data();
    info.enabledExtensionCount = instanceEXT.size();
    info.ppEnabledExtensionNames = instanceEXT.data();

    if (VK_SUCCESS != vkCreateInstance(&info, nullptr, &instance)) {
      FAIL("failed to create instance");
    }
    LOG_SUCCESS;
  }
  SECTION("DEBUG") {
    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.messageSeverity = 0x1111;
    info.messageType = 0xF;
    info.pfnUserCallback = IO::debugCallback;
    info.pUserData = nullptr;
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      if (VK_SUCCESS != func(instance, &info, nullptr, &debugMessenger)) {
        FAIL("failed to create debug messenger");
      }
    } else {
      FAIL("failed to find create debug messenger function");
    }
    LOG_SUCCESS;
  }
  SECTION("SURFACE") {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "my window", nullptr, nullptr);
    if (VK_SUCCESS != glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
      FAIL("failed to create surface");
    }
    LOG_SUCCESS;
  }
  SECTION("DEVICE") {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());
    physicalDevice = devices[0];
    vkGetPhysicalDeviceFeatures(physicalDevice, &pdFeatures);

    VkBool32 is_supported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &is_supported);
    if (is_supported == VK_FALSE) {
      FAIL("surface doesn't support default queue...");
    } else {
      list_blue("surface supports default queue");
    }

    VkDeviceQueueCreateInfo qi{};
    qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qi.queueFamilyIndex = queueFamilyIndex;
    qi.queueCount = 1;
    qi.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &qi;
    info.enabledLayerCount = instanceLAY.size();
    info.ppEnabledLayerNames = instanceLAY.data();
    info.enabledExtensionCount = deviceEXT.size();
    info.ppEnabledExtensionNames = deviceEXT.data();
    info.pEnabledFeatures = &pdFeatures;

    if (VK_SUCCESS != vkCreateDevice(physicalDevice, &info, nullptr, &device)) {
      FAIL("failed to create device");
    }
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
    LOG_SUCCESS;
  }
  SECTION("SWAPCHAIN") {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // skipping clamp since I know the answer
    swapExtent.width = (uint32_t) width;
    swapExtent.height = (uint32_t) height;

    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;
    info.minImageCount = 3;
    info.imageFormat = swapFormat;
    info.imageColorSpace = swapColorSpace;
    info.imageExtent = swapExtent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    info.queueFamilyIndexCount = 1;
    info.pQueueFamilyIndices = &queueFamilyIndex;
    info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    info.clipped = VK_TRUE;
    info.oldSwapchain = VK_NULL_HANDLE;

    if (VK_SUCCESS != vkCreateSwapchainKHR(device, &info, nullptr, &swapchain)) {
      FAIL("failed to create swapchain");
    }
    LOG_SUCCESS;
  }
  SECTION("IMAGE") {
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
    swapImages.resize(count);
    vkGetSwapchainImagesKHR(device, swapchain, &count, swapImages.data());

    swapImageViews.resize(count);
    for (auto i = 0; i < count; i++) {
      VkImageViewCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      info.image = swapImages[i];
      info.viewType = VK_IMAGE_VIEW_TYPE_2D;
      info.format = swapFormat;
      info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
      info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
      info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
      info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
      info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      info.subresourceRange.baseMipLevel = 0;
      info.subresourceRange.levelCount = 1;
      info.subresourceRange.baseArrayLayer = 0;
      info.subresourceRange.layerCount = 1;

      if (VK_SUCCESS != vkCreateImageView(device, &info, nullptr, &swapImageViews[i])) {
        FAIL("failed to create image view for image [" + std::to_string(i) + "]");
      } else {
        list_blue("created image view [" + std::to_string(i) + "]");
      }
    }
    LOG_SUCCESS;
  }
  SECTION("SHADER") {
    const std::string shaderPaths = "../shaders/";
    const std::string vertexFileName = "triangle.vert.spv";
    const std::string fragmentFileName = "red.frag.spv";
    auto vertShaderCode = readFile(shaderPaths + vertexFileName);
    auto fragShaderCode = readFile(shaderPaths + fragmentFileName);
    vertShaderModule = createShaderModule(vertShaderCode);
    fragShaderModule = createShaderModule(fragShaderCode);
    LOG_SUCCESS;
  }
  SECTION("RENDER PASS") {
    /** a simple color buffer that clears the frame-buffer before every draw */

    VkAttachmentDescription colorAttach{};
    colorAttach.format = swapFormat;
    colorAttach.samples = renderSampleCount;
    colorAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // clear frame-buffer before drawing
    colorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkRenderPassCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.pAttachments = &colorAttach;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;

    if (VK_SUCCESS != vkCreateRenderPass(device, &info, nullptr, &renderPass)) {
      FAIL("failed to create render pass");
    }
    LOG_SUCCESS;
  }
  SECTION("PIPELINE") {
    VkPipelineShaderStageCreateInfo vi{};
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vi.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vi.module = vertShaderModule;
    vi.pName = "main"; // entry point
    vi.pSpecializationInfo = VK_NULL_HANDLE;

    VkPipelineShaderStageCreateInfo fi{};
    fi.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fi.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fi.module = fragShaderModule;
    fi.pName = "main";
    fi.pSpecializationInfo = VK_NULL_HANDLE;

    VkPipelineShaderStageCreateInfo shaderStages[] = { vi, fi };

    // describe the format of the vertex data that will be passed to the vertex shader
    //   - using hard-coded vertex shaders, so leaving vertex buffer blank until vertex buffer chapter
    VkPipelineVertexInputStateCreateInfo visi{};
    visi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    visi.vertexBindingDescriptionCount = 0;
    visi.vertexAttributeDescriptionCount = 0;

    // define what kind of geometry will be drawn from the vertices and if primitive restart should be enabled
    VkPipelineInputAssemblyStateCreateInfo iasi{};
    iasi.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    iasi.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    iasi.primitiveRestartEnable = VK_FALSE;

//    VkViewport viewport{};
//    viewport.x = 0;
//    viewport.y = 0;
//    viewport.width = (float) swapExtent.width;
//    viewport.height = (float) swapExtent.height;
//    viewport.minDepth = 0.0f;
//    viewport.maxDepth = 1.0f;
//
//    VkRect2D scissor{};
//    scissor.offset = {0, 0};
//    scissor.extent = swapExtent;

    VkPipelineViewportStateCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewInfo.viewportCount = 1;
    viewInfo.scissorCount = 1;
    // don't pass viewport or scissors unless you want it to be immutable

    VkPipelineRasterizationStateCreateInfo ri {};
    ri.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    ri.depthClampEnable = VK_FALSE;
    ri.rasterizerDiscardEnable = VK_FALSE;
    ri.polygonMode = VK_POLYGON_MODE_FILL;
    ri.lineWidth = 1.0f;
    ri.cullMode = VK_CULL_MODE_BACK_BIT;
    ri.frontFace = VK_FRONT_FACE_CLOCKWISE;
    ri.depthBiasEnable = VK_FALSE;
//    ri.depthBiasConstantFactor = 0.0f;
//    ri.depthBiasClamp = 0.0f;
//    ri.depthBiasSlopeFactor = 0.0f;

    VkPipelineMultisampleStateCreateInfo msi{};
    msi.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msi.sampleShadingEnable = VK_FALSE;
    msi.rasterizationSamples = renderSampleCount;
//    msi.minSampleShading = 1.0f;
//    msi.pSampleMask = nullptr;
//    msi.alphaToCoverageEnable = VK_FALSE;
//    msi.alphaToOneEnable = VK_FALSE;

    /** color blend for frame buffer (v.s. global)
     - basically disabling color blending here

    if (blendEnable) {
        finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
        finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
    } else {
        finalColor = newColor;
    }
    finalColor = finalColor & colorWriteMask;
 */
    VkPipelineColorBlendAttachmentState cba{};
    cba.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    cba.blendEnable = VK_TRUE;
//    cba.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
//    cba.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
//    cba.colorBlendOp = VK_BLEND_OP_ADD;
//    cba.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
//    cba.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
//    cba.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo cbs{};
    cbs.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    cbs.logicOpEnable = VK_FALSE;
    cbs.logicOp = VK_LOGIC_OP_COPY;
    cbs.attachmentCount = 1;
    cbs.pAttachments = &cba;
    cbs.blendConstants[0] = 0.0f;
    cbs.blendConstants[1] = 0.0f;
    cbs.blendConstants[2] = 0.0f;
    cbs.blendConstants[3] = 0.0f;

    // allows adjustments to these states w/out re-creating the pipeline
    std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo di{};
    di.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    di.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    di.pDynamicStates = dynamicStates.data();

    list_blue("creating pipeline layout");
    VkPipelineLayoutCreateInfo pli{};
    pli.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pli.setLayoutCount = 0;
    pli.pushConstantRangeCount = 0;
//    pli.pSetLayouts = nullptr;
//    pli.pPushConstantRanges = nullptr;

    if (VK_SUCCESS != vkCreatePipelineLayout(device, &pli, nullptr, &pipelineLayout)) {
      FAIL("failed to create pipeline layout");
    }

    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.stageCount = 2;
    info.pStages = shaderStages;
    info.pVertexInputState = &visi;
    info.pInputAssemblyState = &iasi;
    info.pViewportState = &viewInfo;
    info.pRasterizationState = &ri;
    info.pMultisampleState = &msi;
//    info.pDepthStencilState = nullptr;
    info.pColorBlendState = &cbs;
    info.pDynamicState = &di;
    info.layout = pipelineLayout;
    info.renderPass = renderPass;
    info.subpass = 0;
    info.basePipelineHandle = VK_NULL_HANDLE;
//    info.basePipelineIndex = -1;

    list_blue("creating graphics pipeline");
    if (VK_SUCCESS != vkCreateGraphicsPipelines(
      device,
      VK_NULL_HANDLE,
      1,
      &info,
      nullptr,
      &pipeline
    )) {
      FAIL("failed to create graphics pipeline");
    }
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    LOG_SUCCESS;
  }

//  while (!glfwWindowShouldClose(window)) {
//    glfwPollEvents();
//  }

  SECTION("DESTROY") {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    for (auto iv : swapImageViews) {
      vkDestroyImageView(device, iv, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    {
      auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
      func(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
    LOG_SUCCESS;
  }
  return EXIT_SUCCESS;
}