#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_metal.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>

#include "lib/snippets/io_macros.h"

#define VK_USE_PLATFORM_METAL_EXT
#define macFLAG VK_KHR_portability_enumeration

GLFWwindow *window = nullptr;
VkInstance instance;
VkDevice device;
VkQueue graphicsQueue;
VkSurfaceKHR surface;
VkDebugUtilsMessengerEXT debugMessenger;

/// EXTENSIONS AND LAYERS
std::vector<const char *> instanceEXT = {
  VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, // FIXME - research
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  /*GLFW extensions go here*/
};
std::vector<const char *> instanceLAY = {
//  "VK_LAYER_LUNARG_api_dump",
//  "VK_LAYER_KHRONOS_profiles",
  "VK_LAYER_KHRONOS_validation",
//  "VK_LAYER_KHRONOS_synchronization2",
//  "VK_LAYER_KHRONOS_shader_object"
};
std::vector<const char *> deviceEXT = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
  /* add "VK_KHR_portability_subset" if device supports it */
};

/// DEVICE PROPERTIES
VkPhysicalDevice physicalDevice;
VkPhysicalDeviceProperties physicalDeviceProperties;
VkPhysicalDeviceFeatures physicalDeviceFeatures;
std::vector<VkQueueFamilyProperties> queueFamilies{};
uint32_t queueFamilyIndex = 0;
float_t queuePriority = 1.0f;


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
  const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
  void*                                            pUserData
) {
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
    std::cerr << "DEBUG MESSENGER : " << pCallbackData->pMessage << std::endl;
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    std::cout << YELLOW << "DEBUG MESSENGER : " << pCallbackData->pMessage << RESET << std::endl;
  } else {
    std::cout << GRAY << "DEBUG MESSENGER : " << pCallbackData->pMessage << RESET << std::endl;
  }
  return VK_FALSE;
}

int main () {
  glfwInit();
  {
    uint32_t count = 0;
    auto glfwEXT = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = 0; i < count; i++) {
      instanceEXT.push_back(glfwEXT[i]);
    }
    for (const auto & ext : instanceEXT) {
      list_gray(ext);
    }
  }

  SECTION("INSTANCE")
  {
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

    auto result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      FAIL("failed to create instance");
    } else {
      LOG_SUCCESS;
    }
  }
  END_SECTION

  SECTION("DEBUG MESSENGER")
  {
    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.pNext = VK_NULL_HANDLE;
    info.messageSeverity = 0x1111;
    info.messageType = 0xF;
    info.pfnUserCallback = debugCallback;
    info.pUserData = VK_NULL_HANDLE;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      auto result = func(instance, &info, nullptr, &debugMessenger);
      if (result != VK_SUCCESS) {
        FAIL("failed to create debug messenger");
      } else {
        LOG_SUCCESS;
      }
    } else {
      FAIL("unable to find function to create debug messenger");
    }
  }
  END_SECTION

  SECTION("PHYSICAL DEVICE")
  {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> options(count);
    vkEnumeratePhysicalDevices(instance, &count, options.data());
    if (options.empty() || options[0] == VK_NULL_HANDLE) {
      FAIL("no device found on machine");
    }
    physicalDevice = options[0];

    /// DEVICE PROPERTIES & FEATURES
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    const auto deviceType = physicalDeviceProperties.deviceType;
    const bool isGPU = deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                       || deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
                       || deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
    const bool isCPU = deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
    std::string deviceTypeLabel = "OTHER";
    if (isGPU) deviceTypeLabel = "GPU";
    if (isCPU) deviceTypeLabel = "CPU";
    const bool supportsGeometryShader = physicalDeviceFeatures.geometryShader != 0;
    std::cout << "> " << physicalDeviceProperties.deviceName << " (" << deviceTypeLabel << ")" << std::endl;
    label_blue("geometryShader", supportsGeometryShader);


    /// DEVICE EXTENSIONS
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> extProperties(count);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, extProperties.data());

//    print(GRAY);
    for (const auto & ext : extProperties) {
//      list_gray(ext.extensionName);
      if (strcmp(ext.extensionName, "VK_KHR_portability_subset") == 0) {
        deviceEXT.push_back("VK_KHR_portability_subset");
      }
    }
//    print(RESET);
    LOG_SUCCESS;
  }
  END_SECTION

  SECTION("PHYSICAL DEVICE QUEUE")
  {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    queueFamilies.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    if (queueFamilies.empty()) {
      FAIL("selected device missing queues...");
    }
    for (const auto & qf : queueFamilies) {
      label("queue count", qf.queueCount);
      std::cout << BLUE;
      label_blue("compute    ", (bool)(qf.queueFlags & VK_QUEUE_COMPUTE_BIT));
      label_blue("graphics   ", (bool)(qf.queueFlags & VK_QUEUE_GRAPHICS_BIT));
      label_blue("transfer   ", (bool)(qf.queueFlags & VK_QUEUE_TRANSFER_BIT));
      std::cout << RESET;
      NEWLINE;
    }
    queueFamilyIndex = 0;
    LOG_SUCCESS;
  }
  END_SECTION

  SECTION("LOGICAL DEVICE")
  {
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
    info.pEnabledFeatures = &physicalDeviceFeatures;

    auto result = vkCreateDevice(physicalDevice, &info, nullptr, &device);
    if (result != VK_SUCCESS) {
      FAIL("unable to create device");
    } else {
      LOG_SUCCESS;
    }
  }
  END_SECTION

  SECTION("RETRIEVE QUEUE HANDLE")
  {
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);
    LOG_SUCCESS;
  }
  END_SECTION

  SECTION("SURFACE")
  {
    VkMetalSurfaceCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
    info.pNext = VK_NULL_HANDLE;
    info.pLayer = VK_NULL_HANDLE;

    auto result = vkCreateMetalSurfaceEXT(instance, &info, nullptr, &surface);
    if (result != VK_SUCCESS) {
      FAIL("failed to create metal surface");
    }
    LOG_SUCCESS;
  }
  END_SECTION

//  window = glfwCreateWindow(800, 600, "hello world", nullptr, nullptr);
//  while (!glfwWindowShouldClose(window)) {
//    glfwPollEvents();
//  }

  print("DESTROYING THE WORLD");
//  glfwDestroyWindow(window);
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyDevice(device, nullptr);
  {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
      func(instance, debugMessenger, nullptr);
    }
  }
  vkDestroyInstance(instance, nullptr);
  glfwTerminate();
  return EXIT_SUCCESS;
}