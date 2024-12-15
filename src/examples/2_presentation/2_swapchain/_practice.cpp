
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>

#include "lib/snippets/io_macros.h"
#include "src/examples/2_presentation/1_window_surface/enum_names.h"

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;

VkDevice device;
VkQueue mainQueue;
VkPhysicalDevice physicalDevice;
VkPhysicalDeviceFeatures physicalDeviceFeatures;
std::vector<VkQueueFamilyProperties> queueFamilies{};
uint32_t queueFamilyIndex = 0;
float_t queuePriority = 1.0f;

GLFWwindow *window = nullptr;
VkSurfaceKHR surface;
VkSurfaceCapabilitiesKHR surfaceCapabilities;
std::vector<VkSurfaceFormatKHR> surfaceFormats{};
std::vector<VkPresentModeKHR> presentModes{};

std::vector<const char *> instanceEXT = {
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  /*GLFW extensions go here*/
};
std::vector<const char *> instanceLAY = {
  "VK_LAYER_KHRONOS_validation",
};
std::vector<const char *> deviceEXT = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME,
  "VK_KHR_portability_subset" // M1 silicon chip
};

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

void glfwErrorCallback(int code, const char* description) {
  std::cerr << RED << "GLFW ERROR " << code << ": " << description << std::endl;
}

int main () {
  SECTION("GLFW") {
    glfwInit();
    glfwSetErrorCallback(glfwErrorCallback);
    uint32_t count = 0;
    auto glfwEXT = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = 0; i < count; i++) {
      instanceEXT.push_back(glfwEXT[i]);
    }
    for (const auto & ext : instanceEXT) {
      list_gray(ext);
    }
  } END_SECTION
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

    auto result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      FAIL("failed to create instance");
    } else {
      LOG_SUCCESS;
    }
  } END_SECTION
  SECTION("DEBUG MESSENGER") {
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
  } END_SECTION
  SECTION("DEVICE") {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> options(count);
    vkEnumeratePhysicalDevices(instance, &count, options.data());
    if (options.empty() || options[0] == VK_NULL_HANDLE) {
      FAIL("no device found on machine");
    }
    physicalDevice = options[0];
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

    list_blue(physicalDeviceProperties.deviceName);

    count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    queueFamilies.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    if (queueFamilies.empty()) {
      FAIL("selected device missing queues...");
    }
    queueFamilyIndex = 0;
    queuePriority = 1.0f;

    for (const auto & ext : deviceEXT) {
      list_gray(ext);
    }

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
    info.enabledExtensionCount = deviceEXT.size();
    info.ppEnabledExtensionNames = deviceEXT.data();
    info.pEnabledFeatures = &physicalDeviceFeatures;

    auto result = vkCreateDevice(physicalDevice, &info, nullptr, &device);
    if (result != VK_SUCCESS) {
      FAIL("unable to create device");
    }
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &mainQueue);
    LOG_SUCCESS;
  } END_SECTION
  SECTION("WINDOW") {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);
    LOG_SUCCESS;
  } END_SECTION
  SECTION("SURFACE") {
    auto result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    if (result != VK_SUCCESS) {
      FAIL("failed to create window surface");
    }

    SECTION("  Queue Family Surface Support:") {
      for (auto i = 0; i < queueFamilies.size(); i++) {
        VkBool32 supported;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supported);
        if (supported) {
          std::cout << GREEN << "    + queue family " << i << " supports surface" <<  RESET << std::endl;
        } else {
          std::cout << RED   << "   - queue family " << i << " does not support surface" << RESET << std::endl;
        }
      }

      VkBool32 surfaceSupported;
      vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &surfaceSupported);
      if (surfaceSupported == VK_FALSE) {
        FAIL("surface not supported for selected queue family");
      }
      NEWLINE;
    } END_SECTION

    SECTION("  Surface Capabilities:") {
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

      auto sc = &surfaceCapabilities;
      std::cout << GRAY;
      std::cout << "    - image count    : " << sc->minImageCount << "-" << sc->maxImageCount << std::endl;
      std::cout << "    - currentExtent  : " << sc->currentExtent.height << "h x " << sc->currentExtent.width << "w " << std::endl;
      std::cout << "    - minImageExtent : " << sc->minImageExtent.height << "h x " << sc->minImageExtent.width << "w " << std::endl;
      std::cout << "    - maxImageExtent : " << sc->maxImageExtent.height << "h x " << sc->maxImageExtent.width << "w " << std::endl;
      std::cout << "    - maxImageArrayLayers : " << sc->maxImageArrayLayers << std::endl;
      // the rest are flags. I'll do them later if they ever become important
      std::cout << RESET;
      NEWLINE;
    } END_SECTION

    SECTION("  Present Modes:") {
      uint32_t count = 0;
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, nullptr);
      presentModes.resize(count);
      vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &count, presentModes.data());
      std::cout << GRAY;
      for (const auto & present : presentModes) {
        std::cout << "    - " << presentModeFlagName(present) << std::endl;
      }
      std::cout << RESET;
      NEWLINE;
    } END_SECTION

    SECTION("  Formats:") {
      uint32_t count = 0;
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, nullptr);
      surfaceFormats.resize(count);
      vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &count, surfaceFormats.data());
      std::cout << GRAY;
      for (const auto &format: surfaceFormats) {
        std::cout << "    - " << colorSpaceFlagName(format.colorSpace)
                  << " - "    << formatFlagName(format.format)
                  << std::endl;
      }
      std::cout << RESET;
      NEWLINE;
    } END_SECTION

    if (surfaceFormats.empty()) {
      std::cerr << RED << "no surface formats found" << RESET << std::endl;
    }
    if (presentModes.empty()) {
      std::cerr << RED << "no present modes found" << RESET << std::endl;
    }
    if (surfaceFormats.empty() || presentModes.empty()) {
      FAIL("surface failed");
    }
    LOG_SUCCESS;
  } END_SECTION
  SECTION("SWAPCHAIN") /*CODE ME*/ {

    LOG_FAILURE;
  } END_SECTION


  SECTION("DESTROYING THE WORLD") {
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);
    {
      auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT"
      );
      if (func != nullptr) {
        func(instance, debugMessenger, nullptr);
      }
    }
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
    LOG_SUCCESS;
  } END_SECTION
  return EXIT_SUCCESS;
}