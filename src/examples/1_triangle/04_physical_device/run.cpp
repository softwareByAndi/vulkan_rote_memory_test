#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <vector>
#include <optional>
#include "lib/snippets/io_macros.h";
#include "lib/snippets/useful_functions.h";

// #define LOG_SUCCESS std::cout << "  - SUCCESS" << std::endl << std::endl
// #define LOG_FAILURE std::cout << "  - FAILURE" << std::endl << std::endl
// #define FAIL(x) LOG_FAILURE; throw std::runtime_error(x)
// #define NEWLINE std::cout << std::endl
// #define print(x) std::cout << x << std::endl
// #define list(x) std::cout << "  - " << x << std::endl
// #define DNL std::endl << std::endl;

#define macEXT VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
VkInstance instance;
std::vector<const char*> reqEXT = {
  macEXT
};
std::vector<const char*> reqLAY = {
  "VK_LAYER_KHRONOS_validation"
};

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkPhysicalDeviceProperties properties;
VkPhysicalDeviceFeatures features;
std::vector<VkQueueFamilyProperties> queueFamilies{};
uint32_t queueFamilyIndex = 0;


int main() {
  SECTION("CREATING INSTANCE") {
    VkApplicationInfo ai{};
    ai.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    ai.pNext = nullptr;
    ai.pApplicationName = "test app";
    ai.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.pEngineName = "no engine";
    ai.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    ai.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = nullptr;
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    info.pApplicationInfo = &ai;
    info.enabledLayerCount = reqLAY.size();
    info.ppEnabledLayerNames = reqLAY.data();
    info.enabledExtensionCount = reqEXT.size();
    info.ppEnabledExtensionNames = reqEXT.data();

    if (VK_SUCCESS != vkCreateInstance(&info, nullptr, &instance)) {
      FAIL("failed to create instance");
    }
    LOG_SUCCESS;
  }
  SECTION("SELECTING PHYSICAL DEVICE") {
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());
    if (devices.empty() || devices[0] == VK_NULL_HANDLE) {
      FAIL("failed to find a device");
    }
    physicalDevice = devices[0];

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    const bool isCPU = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
    const bool isGPU = properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                       || properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
                       || properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
    std::string deviceType = "OTHER";
    if (isCPU) deviceType = "CPU";
    if (isGPU) deviceType = "GPU";
    std::cout << "> " << properties.deviceName << std::endl;
    label_blue("deviceType", deviceType);
    label_blue("geometryShader", features.geometryShader);
    if (!isGPU) {
      FAIL("selected device is not a GPU");
    }
    LOG_SUCCESS;
  }
  SECTION("QUEUE FAMILIES") {
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    queueFamilies.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    if (queueFamilies.empty()) {
      FAIL("selected device doesn't have any queues...");
    }
    for (const auto & family : queueFamilies) {
      const auto supportsGraphics = family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
      label_blue("queue count", family.queueCount);
      label_blue("supports graphics", supportsGraphics);
    }
    LOG_SUCCESS;
  }


  SECTION("DESTROYING THE WORLD") {
    vkDestroyInstance(instance, nullptr);
    LOG_SUCCESS;
  }
  return EXIT_SUCCESS;
}