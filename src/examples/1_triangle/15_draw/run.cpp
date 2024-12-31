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

VkViewport viewport{};
VkRect2D scissor{};

const VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
uint32_t currentImageIndex = 0;
VkShaderModule vertexShader;
VkShaderModule fragmentShader;
VkRenderPass renderPass;
VkPipelineLayout pipelineLayout;
VkPipeline pipeline;
std::vector<VkDynamicState> pipelineDynamicStates = {
  VK_DYNAMIC_STATE_VIEWPORT,
  VK_DYNAMIC_STATE_SCISSOR
};
std::vector<VkFramebuffer> swapFramebuffers;
VkCommandPool commandPool;
VkCommandBuffer commandBuffer;
VkSemaphore semaphoreIMG;
VkSemaphore semaphoreRENDER;
VkFence fenceIMG;

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
VkShaderModule createShaderModule(const std::string& fileName) {
  auto code = readFile(fileName);
  VkShaderModuleCreateInfo info{};
  info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  info.codeSize = code.size();
  info.pCode = reinterpret_cast<const uint32_t*>(code.data());
  VkShaderModule shader;
  if (VK_SUCCESS != vkCreateShaderModule(device, &info, nullptr, &shader)) {
    FAIL("failed to create shader module");
  }
  return shader;
}



VKAPI_ATTR VkResult VKAPI_CALL recordCommandBuffer(uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  beginInfo.pInheritanceInfo = nullptr;
  if (VK_SUCCESS != vkBeginCommandBuffer(commandBuffer, &beginInfo)) {
    FAIL("failed to begin command buffer");
  }
  VkRenderPassBeginInfo renderInfo{};
  renderInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderInfo.renderPass = renderPass;
  renderInfo.framebuffer = swapFramebuffers[imageIndex];
  renderInfo.renderArea.offset = {0, 0};
  renderInfo.renderArea.extent = swapExtent;
  renderInfo.clearValueCount = 1;
  renderInfo.pClearValues = &clearColor;
  vkCmdBeginRenderPass(commandBuffer, &renderInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(swapExtent.width);
  viewport.height = static_cast<float>(swapExtent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  scissor.offset = {0, 0};
  scissor.extent = swapExtent;

  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  vkCmdDraw(commandBuffer, 3, 1, 0, 0);
  vkCmdEndRenderPass(commandBuffer);
  return vkEndCommandBuffer(commandBuffer);
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
  SECTION("IMAGE VIEWS") {
    uint32_t count = 0;
    vkGetSwapchainImagesKHR(device, swapchain, &count, nullptr);
    swapImages.resize(count);
    swapImageViews.resize(count);
    vkGetSwapchainImagesKHR(device, swapchain, &count, swapImages.data());

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
        FAIL("failed to create image view");
      }
    }
    LOG_SUCCESS;
  }
  SECTION("SHADERS") {
    const std::string shaderDir = "../shaders/";
    const std::string vertexFileName = "triangle.vert.spv";
//    const std::string fragmentFileName = "red.frag.spv";
    const std::string fragmentFileName = "interpolate_color.frag.spv";
    vertexShader = createShaderModule(shaderDir + vertexFileName);
    fragmentShader = createShaderModule(shaderDir + fragmentFileName);
    LOG_SUCCESS;
  }
  SECTION("RENDER PASS") {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0; // index
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

//    VkSubpassDependency dependency{};
//    dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // implicit index to previous subpass
//    dependency.dstSubpass = 0; // index to current subpass
//    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait on this
//    dependency.srcAccessMask = 0;
//    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // operation that's waiting
//    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // action that's being performed

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependency.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    info.attachmentCount = 1;
    info.subpassCount = 1;
    info.pAttachments = &colorAttachment;
    info.pSubpasses = &subpass;
    info.dependencyCount = 1;
    info.pDependencies = &dependency;
    if (VK_SUCCESS != vkCreateRenderPass(device, &info, nullptr, &renderPass)) {
      FAIL("failed to create render pass");
    }
    LOG_SUCCESS;
  }
  SECTION("PIPELINE") {
    VkPipelineShaderStageCreateInfo vi{};
    vi.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vi.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vi.module = vertexShader;
    vi.pName = "main";
    vi.pSpecializationInfo = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo fi{};
    fi.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fi.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fi.module = fragmentShader;
    fi.pName = "main";
    fi.pSpecializationInfo = VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo shaderStages[] = { vi, fi };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.scissorCount = 1;
    viewportInfo.viewportCount = 1;
    // don't ref unless immutable

    VkPipelineRasterizationStateCreateInfo rasterizeInfo{};
    rasterizeInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizeInfo.depthClampEnable = VK_FALSE;
    rasterizeInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizeInfo.polygonMode = VK_POLYGON_MODE_FILL;
//    rasterizeInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizeInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizeInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizeInfo.depthBiasEnable = VK_FALSE;
    rasterizeInfo.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampleInfo{};
    multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleInfo.sampleShadingEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorAttachment{};
    colorAttachment.blendEnable = VK_FALSE;
    colorAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                     VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


    VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorAttachment;
    colorBlendInfo.blendConstants[0] = 0.0f;
    colorBlendInfo.blendConstants[1] = 0.0f;
    colorBlendInfo.blendConstants[2] = 0.0f;
    colorBlendInfo.blendConstants[3] = 0.0f;

    VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = pipelineDynamicStates.size();
    dynamicStateInfo.pDynamicStates = pipelineDynamicStates.data();

    VkPipelineLayoutCreateInfo playInfo{};
    playInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    playInfo.setLayoutCount = 0;
    playInfo.pushConstantRangeCount = 0;
    if (VK_SUCCESS != vkCreatePipelineLayout(device, &playInfo, nullptr, &pipelineLayout)) {
      FAIL("failed to create pipeline layout");
    }
    VkGraphicsPipelineCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    info.stageCount = 2;
    info.pStages = shaderStages;
    info.pVertexInputState = &vertexInputInfo;
    info.pInputAssemblyState = &inputAssemblyInfo;
    info.pTessellationState = nullptr;
    info.pViewportState = &viewportInfo;
    info.pRasterizationState = &rasterizeInfo;
    info.pMultisampleState = &multisampleInfo;
    info.pDepthStencilState = nullptr;
    info.pColorBlendState = &colorBlendInfo;
    info.pDynamicState = &dynamicStateInfo;
    info.layout = pipelineLayout;
    info.renderPass = renderPass;
    info.subpass = 0; // index
    info.basePipelineHandle = VK_NULL_HANDLE;

    if (VK_SUCCESS != vkCreateGraphicsPipelines(
      device,
      nullptr,
      1,
      &info,
      nullptr,
      &pipeline
    )) {
      FAIL("failed to create graphics pipeline");
    }
    vkDestroyShaderModule(device, vertexShader, nullptr);
    vkDestroyShaderModule(device, fragmentShader, nullptr);
    LOG_SUCCESS;
  }s
  SECTION("FRAME BUFFER") {
    swapFramebuffers.resize(swapImageViews.size());
    for (auto i = 0; i < swapImageViews.size(); i++) {
      VkImageView attachments[] = {
        swapImageViews[i]
      };
      VkFramebufferCreateInfo info{};
      info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      info.renderPass = renderPass;
      info.attachmentCount = 1;
      info.pAttachments = attachments;
      info.width = swapExtent.width;
      info.height = swapExtent.height;
      info.layers = 1;
      if (VK_SUCCESS != vkCreateFramebuffer(device, &info, nullptr, &swapFramebuffers[i])) {
        FAIL("failed to create frame buffer");
      }
      list_blue("created framebuffer for imageview [" + std::to_string(i) + "]");
    }
    LOG_SUCCESS;
  }
  SECTION("ALLOCATE COMMAND POOL") {
    VkCommandPoolCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = queueFamilyIndex;
    if (VK_SUCCESS != vkCreateCommandPool(device, &info, nullptr, &commandPool)) {
      FAIL("failed to create command pool");
    }
    VkCommandBufferAllocateInfo comalloc{};
    comalloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    comalloc.commandPool = commandPool;
    comalloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    comalloc.commandBufferCount = 1;
    if (VK_SUCCESS != vkAllocateCommandBuffers(device, &comalloc, &commandBuffer)) {
      FAIL("failed to allocate command buffer");
    }
    LOG_SUCCESS;
  }
  SECTION("SYNC") {
    VkSemaphoreCreateInfo si{};
    si.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    si.flags = VK_SEMAPHORE_TYPE_BINARY;
    VkFenceCreateInfo fi{};
    fi.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fi.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    if (
      VK_SUCCESS != vkCreateSemaphore(device, &si, nullptr, &semaphoreIMG)
      || VK_SUCCESS != vkCreateSemaphore(device, &si, nullptr, &semaphoreRENDER)
      || VK_SUCCESS != vkCreateFence(device, &fi, nullptr, &fenceIMG)
      ) {
      FAIL("failed to create sync objects");
    }
    LOG_SUCCESS;
  }
  SECTION("DRAW") {
    VkSwapchainKHR swapchains[] = {swapchain};
    VkSemaphore waitSemaphores[] = {semaphoreIMG};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = {semaphoreRENDER};

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      auto waitFenceResult = vkWaitForFences(device, 1, &fenceIMG, VK_TRUE, UINT64_MAX);
      if (waitFenceResult != VK_SUCCESS) {
        std::cout << "image result: " << waitFenceResult << std::endl;
      }
      auto resetFenceResult = vkResetFences(device, 1, &fenceIMG);
      if (resetFenceResult != VK_SUCCESS) {
        std::cout << "image result: " << resetFenceResult << std::endl;
      }
      auto imgResult = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, semaphoreIMG, VK_NULL_HANDLE, &currentImageIndex);
      if (imgResult != VK_SUCCESS) {
        std::cout << "image result: " << imgResult << std::endl;
      }

      auto resetResult = vkResetCommandBuffer(commandBuffer, 0);
      if (resetResult != VK_SUCCESS) {
        std::cout << "reset result: " << resetResult << std::endl;
      }
      if (VK_SUCCESS != recordCommandBuffer(currentImageIndex)) {
        FAIL("failed to record command buffer");
      }


      VkSubmitInfo submitInfo{};
      submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      submitInfo.waitSemaphoreCount = 1;
      submitInfo.pWaitSemaphores = waitSemaphores;
      submitInfo.pWaitDstStageMask = waitStages;
      submitInfo.commandBufferCount = 1;
      submitInfo.pCommandBuffers = &commandBuffer;
      submitInfo.signalSemaphoreCount = 1;
      submitInfo.pSignalSemaphores = signalSemaphores;
      if (VK_SUCCESS != vkQueueSubmit(queue, 1, &submitInfo, fenceIMG)) {
        FAIL("failed to submit queue");
      }

      VkPresentInfoKHR presentInfo{};
      presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      presentInfo.waitSemaphoreCount = 1;
      presentInfo.pWaitSemaphores = signalSemaphores;
      presentInfo.swapchainCount = 1;
      presentInfo.pSwapchains = swapchains;
      presentInfo.pImageIndices = &currentImageIndex;
      presentInfo.pResults = nullptr; // optional
      auto presentResult = vkQueuePresentKHR(queue, &presentInfo);
      if (presentResult != VK_SUCCESS) {
        std::cout << "present result: " << presentResult << std::endl;
      }

      vkDeviceWaitIdle(device);
    }
  }

  SECTION("DESTROY") {
    vkDeviceWaitIdle(device);
    vkDestroySemaphore(device, semaphoreIMG, nullptr);
    vkDestroySemaphore(device, semaphoreRENDER, nullptr);
    vkDestroyFence(device, fenceIMG, nullptr);
    vkDestroyCommandPool(device, commandPool, nullptr);
    for (auto framebuffer : swapFramebuffers) {
      vkDestroyFramebuffer(device, framebuffer, nullptr);
    }
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
    for (auto imageView : swapImageViews) {
      vkDestroyImageView(device, imageView, nullptr);
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