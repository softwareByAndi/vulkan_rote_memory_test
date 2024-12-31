#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include "lib/snippets/io_macros.h"
#include "lib/snippets/useful_functions.h"

GLFWwindow *window = nullptr;
VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
std::vector<const char *> instanceEXT = {
  // "TEST_fake_extension",
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, // MAC
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  /*GLFW extensions go here*/
};
std::vector<const char *> instanceLAY = {
  // "TEST_fake_extension",
  "VK_LAYER_KHRONOS_validation",
//  "VK_LAYER_LUNARG_api_dump",
//  "VK_LAYER_KHRONOS_profiles",
//  "VK_LAYER_KHRONOS_synchronization2",
//  "VK_LAYER_KHRONOS_shader_object"
};

VkDevice device;
VkPhysicalDevice physDevice;
VkPhysicalDeviceProperties physDeviceProperties;
VkPhysicalDeviceFeatures physDeviceFeatures;
std::vector<VkQueueFamilyProperties> queueFamilies{};
uint32_t queueFamilyIndex = 0;
float_t queuePriority = 1.0f;
std::vector<const char *> deviceEXT = {
  // "TEST_fake_extension",
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  /* add "VK_KHR_portability_subset" if device supports it */
};

VkSurfaceKHR surface;
VkSurfaceCapabilitiesKHR surfCaps;

VkSwapchainKHR swapchain;
uint32_t imageCount = 3;
VkFormat swapFormat = VK_FORMAT_B8G8R8A8_SRGB;
VkColorSpaceKHR swapColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
VkExtent2D swapExtent;
VkRect2D swapViewport;
std::vector<VkImage> swapImages(imageCount);
std::vector<VkImageView> swapImageViews(imageCount);


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
  const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
  void*                                            pUserData
) {
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    std::cerr << RED << "DEBUG MESSENGER : " << pCallbackData->pMessage << RESET << std::endl;
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    std::cout << YELLOW << "DEBUG MESSENGER : " << pCallbackData->pMessage << RESET << std::endl;
  } else {
    std::cout << GRAY << "DEBUG MESSENGER : " << pCallbackData->pMessage << RESET << std::endl;
  }
  return VK_FALSE;
}

int main () {
  SECTION("GLFW") {
    glfwInit();
    uint32_t count = 0;
    auto glfwEXT = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = 0; i < count; i++) {
      instanceEXT.push_back(glfwEXT[i]);
    }
    LOG_SUCCESS;
  }
  SECTION("EXTENSIONS") {
    uint32_t count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, availableExtensions.data());
    for (const auto &[extensionName, _v]: availableExtensions) {
      if (UF::includes(instanceEXT, extensionName)) list_blue(extensionName);
      else list_gray(extensionName);
    }
    bool extensionsNotFound = false;
    for (const auto &extensionName : instanceEXT) {
      if (!UF::includes(availableExtensions, extensionName)) {
        extensionsNotFound = true;
        list_red(extensionName);
      }
    }
    if (extensionsNotFound) {
      FAIL("required extensions not found");
    }
    LOG_SUCCESS;
  }
  SECTION("LAYERS") {
    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    std::vector<VkLayerProperties> availableLayers(count);
    vkEnumerateInstanceLayerProperties(&count, availableLayers.data());
    for (const auto &[layerName, _x, _y, _z]: availableLayers) {
      if (UF::includes(instanceLAY, layerName)) list_blue(layerName);
      else list_gray(layerName);
    }
    bool layersNotFound = false;
    for (const auto &layerName : instanceLAY) {
      if (!UF::includes(availableLayers, layerName)) {
        layersNotFound = true;
        list_red(layerName);
      }
    }
    if (layersNotFound) {
      FAIL("required extensions not found");
    }
    LOG_SUCCESS;
  }
  SECTION("INSTANCE") {
    VkApplicationInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pNext = VK_NULL_HANDLE;
    ai.pApplicationName = "new game";
    ai.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.pEngineName = "no engine";
    ai.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = VK_NULL_HANDLE;
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
  SECTION("DEBUG MESSENGER") {
    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.pNext = VK_NULL_HANDLE;
    info.messageSeverity = 0x1111;
    info.messageType = 0xF;
    info.pfnUserCallback = debugCallback;
    info.pUserData = VK_NULL_HANDLE;

    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func == nullptr) {
      FAIL("unable to find function to create debug messenger");
    }
    if (VK_SUCCESS != func(instance, &info, nullptr, &debugMessenger)) {
        FAIL("failed to create debug messenger");
    }
    LOG_SUCCESS;
  }
  SECTION("PHYSICAL DEVICE") {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> options(count);
    vkEnumeratePhysicalDevices(instance, &count, options.data());
    if (options.empty() || options[0] == VK_NULL_HANDLE) {
      FAIL("no device found on machine");
    }
    physDevice = options[0];

    /// DEVICE PROPERTIES & FEATURES
    vkGetPhysicalDeviceFeatures(physDevice, &physDeviceFeatures);
    vkGetPhysicalDeviceProperties(physDevice, &physDeviceProperties);
    const auto deviceType = physDeviceProperties.deviceType;
    const bool isGPU = deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                       || deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
                       || deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
    const bool isCPU = deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
    std::string deviceTypeLabel = "OTHER";
    if (isGPU) deviceTypeLabel = "GPU";
    if (isCPU) deviceTypeLabel = "CPU";
    const bool supportsGeometryShader = physDeviceFeatures.geometryShader != 0;
    std::cout << "> " << physDeviceProperties.deviceName << " (" << deviceTypeLabel << ")" << std::endl;
    label_blue("geometryShader", supportsGeometryShader);
    LOG_SUCCESS;
  }
  SECTION("DEVICE EXTENSIONS") {
    uint32_t count = 0;
    vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> deviceExtensions(count);
    vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &count, deviceExtensions.data());
    for (const auto &[extensionName, _v] : deviceExtensions) {
      if (strcmp(extensionName, "VK_KHR_portability_subset") == 0) {
        deviceEXT.push_back("VK_KHR_portability_subset");
      }
    }
    for (const auto&[extensionName, _v]: deviceExtensions) {
      if (UF::includes(deviceEXT, extensionName)) list_blue(extensionName);
      else list_gray(extensionName);
    }
    bool extensionsNotFound = false;
    for (const auto& extensionName : deviceEXT) {
      if (!UF::includes(deviceExtensions, extensionName)) {
        extensionsNotFound = true;
        list_red(extensionName);
      }
    }
    if (extensionsNotFound) {
      FAIL("required extensions not found");
    }
    LOG_SUCCESS;
  }
  SECTION("PHYSICAL DEVICE QUEUE") {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &count, nullptr);
    queueFamilies.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &count, queueFamilies.data());
    if (queueFamilies.empty()) {
      FAIL("selected device missing queues...");
    }
    for (const auto & qf : queueFamilies) {
      label("queue count", qf.queueCount);
      label_blue("compute    ", static_cast<bool>(qf.queueFlags & VK_QUEUE_COMPUTE_BIT));
      label_blue("graphics   ", static_cast<bool>(qf.queueFlags & VK_QUEUE_GRAPHICS_BIT));
      label_blue("transfer   ", static_cast<bool>(qf.queueFlags & VK_QUEUE_TRANSFER_BIT));
      NEWLINE;
    }
    queueFamilyIndex = 0;
    LOG_SUCCESS;
  }
  SECTION("LOGICAL DEVICE") {
    VkDeviceQueueCreateInfo qi{};
    qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qi.pNext = VK_NULL_HANDLE;
    qi.queueFamilyIndex = queueFamilyIndex;
    qi.queueCount = 1;
    qi.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pNext = VK_NULL_HANDLE;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &qi;
    info.enabledLayerCount = instanceLAY.size();
    info.ppEnabledLayerNames = instanceLAY.data();
//    info.enabledExtensionCount = 0;
    info.enabledExtensionCount = deviceEXT.size();
    info.ppEnabledExtensionNames = deviceEXT.data();
    info.pEnabledFeatures = &physDeviceFeatures;

    if (VK_SUCCESS != vkCreateDevice(physDevice, &info, nullptr, &device)) {
      FAIL("unable to create device");
    }
    LOG_SUCCESS;
  }
  SECTION("RETRIEVE QUEUE HANDLE") {
    /* there's nothing to use this handle on yet, so just adding it here as reference. */
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);
    LOG_SUCCESS;
  }
  SECTION("DESTROYING THE WORLD") {
    vkDestroyDevice(device, nullptr);
    {
      auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
      );
      if (func != nullptr) {
        func(instance, debugMessenger, nullptr);
      }
    }
    vkDestroyInstance(instance, nullptr);
    glfwTerminate();
    LOG_SUCCESS;
  }
  return EXIT_SUCCESS;
}