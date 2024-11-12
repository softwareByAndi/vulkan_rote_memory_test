#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "lib/snippets/io_macros.h"

VkInstance instance;
VkDevice device;
std::vector<const char *> instanceEXT = {
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
  /*GLFW extensions go here*/
};
std::vector<const char *> instanceLAY = {
  "VK_LAYER_KHRONOS_validation"
};

std::vector<const char *> deviceEXT = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME, /*requires GLFW extensions be added to instance*/
  /* add "VK_KHR_portability_subset" if device supports it */
};
VkPhysicalDevice physicalDevice;
VkPhysicalDeviceFeatures physicalDeviceFeatures;
VkPhysicalDeviceProperties physicalDeviceProperties;
std::vector<VkQueueFamilyProperties> queueFamilies{};
uint32_t queueFamilyIndex = 0;
std::vector<float> queuePriorities = {};

int main () {
  glfwInit();
  {
    uint32_t count = 0;
    auto glfwEXT = glfwGetRequiredInstanceExtensions(&count);
    for (auto i = 0; i < count; i++) {
      instanceEXT.push_back(glfwEXT[i]);
    }
  }

  {
    print("INSTANCE");
    VkApplicationInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pNext = nullptr;
    ai.pApplicationName = "app";
    ai.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.pEngineName = "engine";
    ai.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags |= VK_KHR_portability_enumeration;
    info.pApplicationInfo = &ai;
    info.enabledLayerCount = instanceLAY.size();
    info.ppEnabledLayerNames = instanceLAY.data();
    info.enabledExtensionCount = instanceEXT.size();
    info.ppEnabledExtensionNames = instanceEXT.data();

    auto result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      FAIL("failed to create instance");
    }
    LOG_SUCCESS;
  }

  {
    print("PHYSICAL DEVICE");
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> hardware(count);
    vkEnumeratePhysicalDevices(instance, &count, hardware.data());
    if (hardware.empty() || hardware[0] == VK_NULL_HANDLE || hardware[0] == nullptr) {
      FAIL("failed to find a device");
    }
    physicalDevice = hardware[0];
    /// FEATURES & PROPERTIES
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    /// QUEUE
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    queueFamilies.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    queueFamilyIndex = 0;
    for (auto i = 0; i < queueFamilies[queueFamilyIndex].queueCount; i++) {
      queuePriorities.push_back(1.0f);
    }
    /// DEVICE EXTENSIONS
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> props(count);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, props.data());
    for (const auto & prop : props) {
      if (strcmp(prop.extensionName, "VK_KHR_portability_subset") == 0) {
        deviceEXT.push_back("VK_KHR_portability_subset");
      }
    }
    LOG_SUCCESS;
  }

  {
    print("LOGICAL DEVICE");
    VkDeviceQueueCreateInfo qi{};
    qi.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qi.queueFamilyIndex = queueFamilyIndex;
    qi.queueCount = queueFamilies[queueFamilyIndex].queueCount; // there's only one queue/family on M1 chip anyway
    qi.pQueuePriorities = queuePriorities.data();

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.queueCreateInfoCount = 1;
    info.pQueueCreateInfos = &qi;
    info.enabledLayerCount = instanceLAY.size();
    info.ppEnabledLayerNames = instanceLAY.data();
    info.enabledExtensionCount = deviceEXT.size();
    info.ppEnabledExtensionNames = deviceEXT.data();
    info.pEnabledFeatures = VK_NULL_HANDLE;

    auto result = vkCreateDevice(physicalDevice, &info, nullptr, &device);
    if (result != VK_SUCCESS) {
      FAIL("failed to create device");
    }
    LOG_SUCCESS;
  }

  {
    print("RETRIEVE QUEUE HANDLE");
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &graphicsQueue);
    LOG_SUCCESS;
  }

  print("DESTROYING THE WORLD");
  vkDestroyDevice(device, nullptr);
  vkDestroyInstance(instance, nullptr);
  glfwTerminate();
  return EXIT_SUCCESS;
}