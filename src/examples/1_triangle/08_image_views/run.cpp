#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include "lib/snippets/useful_functions.h"
#include "lib/snippets/vulkan_io.h"

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
std::vector<const char *> instanceEXT {
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
std::vector<const char *> instanceLAY {
  "VK_LAYER_KHRONOS_validation"
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


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
  const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
  void*                                            pUserData
) {
  std::cout << GRAY << "DEBUG - ";

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    std::cout << GRAY << "VERBOSE - ";
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    std::cout << GRAY << "INFO - ";
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    std::cout << YELLOW << "WARNING - ";
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    std::cerr << RED << "ERROR - ";

  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    std::cout << "GENERAL - ";
  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    std::cout << "VALIDATION - ";
  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    std::cout << "PERFORMANCE - ";
  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
    std::cout << "ADDRESS BINDING - ";

  std::cout << pCallbackData->pMessage << RESET << std::endl;
  return VK_FALSE;
}

int main() {
  SECTION("GLFW") {
    glfwInit();
    uint32_t count = 0;
    auto glfwEXT = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = 0; i < count; i++) {
      instanceEXT.push_back(glfwEXT[i]);
    }
  }
  SECTION("INSTANCE") {
    print("  EXTENSIONS");
    if (VK_SUCCESS != IO::checkInstanceExtensions(instanceEXT)) {
      FAIL("missing required extensions");
    }
    print("  LAYERS");
    if (VK_SUCCESS != IO::checkInstanceLayers(instanceLAY)) {
      FAIL("missing required layers");
    }

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
    info.pfnUserCallback = debugCallback;
    info.pUserData = nullptr;
    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
    );
    if (func == nullptr) {
      FAIL("failed to find create debug messenger function");
    }
    if (VK_SUCCESS != func(instance, &info, nullptr, &debugMessenger)) {
      FAIL("failed to create debug messenger");
    }
    LOG_SUCCESS;
  }
  SECTION("DEVICE") {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());
    physicalDevice = devices[0];
    VkPhysicalDeviceProperties pdProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &pdProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &pdFeatures);

    list_blue(pdProperties.deviceName);
    print("  EXTENSIONS");
    if (VK_SUCCESS != IO::checkDeviceExtensions(physicalDevice, deviceEXT)) {
      FAIL("missing required extensions");
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
  SECTION("SURFACE") {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "my window", nullptr, nullptr);
    if (VK_SUCCESS != glfwCreateWindowSurface(instance, window, nullptr, &surface)) {
      FAIL("failed to create surface");
    }
    VkBool32 surfaceIsSupported;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &surfaceIsSupported);
    if (surfaceIsSupported == VK_FALSE) {
      FAIL("surface not supported for selected queue family");
    }
    list_blue("surface is supported");
    LOG_SUCCESS;
  }
  SECTION("SWAPCHAIN") {
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfCaps);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    // skipping clamp since I know the answer
    swapExtent.width = static_cast<uint32_t>(width);
    swapExtent.height = static_cast<uint32_t>(height);

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
      }
      list_blue("created image view [" + std::to_string(i) + "]");
    }
    LOG_SUCCESS;
  }
  SECTION("DESTROY") {
    for (auto iv : swapImageViews) {
      vkDestroyImageView(device, iv, nullptr);
    }
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    {
      const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
      );
      if (func != nullptr) {
        func(instance, debugMessenger, nullptr);
      } else {
        error("failure to find vkDestroyDebugUtilsMessengerEXT");
      }
    }
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
    LOG_SUCCESS;
  }
  return EXIT_SUCCESS;
}