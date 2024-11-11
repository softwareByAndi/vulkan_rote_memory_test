/**
 * PRACTICE - PHYSICAL DEVICE
 *
 * TASKS
 * - find physical devices on this computer
 * > for each device
 *     - print device name
 *     - print if device supports geometryShader
 *     - query queue families
 *     > for each queue family
 *         - print queue count
 *         - print if it supports graphics
 * - select a physical device
 * - select a queue family which supports graphics
 */

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <vector>
#include <optional>
#include <string>

#define LOG_SUCCESS std::cout << "  - SUCCESS" << std::endl << std::endl
#define LOG_FAILURE std::cout << "  - FAILURE" << std::endl << std::endl
#define NEWLINE std::cout << std::endl
#define print(x) std::cout << x << std::endl
#define list(x) std::cout << "  - " << x << std::endl

#define macEXT VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
#define macFLAG VK_KHR_portability_enumeration
VkInstance instance;
std::vector<const char*> reqEXT = { macEXT };
std::vector<const char*> reqLAY = { "VK_LAYER_KHRONOS_validation" };

VkPhysicalDevice physicalDevice;
VkPhysicalDeviceProperties deviceProperties;
VkPhysicalDeviceFeatures deviceFeatures;
std::vector<VkQueueFamilyProperties> queueFamilies{};
uint32_t queueFamilyIndex = 0;

int main() {
  {
    print("CREATING INSTANCE");
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
    info.flags |= macFLAG;
    info.pApplicationInfo = &ai;
    info.enabledLayerCount = reqLAY.size();
    info.ppEnabledLayerNames = reqLAY.data();
    info.enabledExtensionCount = reqEXT.size();
    info.ppEnabledExtensionNames = reqEXT.data();

    auto result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      LOG_FAILURE;
      throw std::runtime_error("failed to create instance");
    }
    LOG_SUCCESS;
  }

  {
    print("LISTING PHYSICAL DEVICES");
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> hardware(count);
    vkEnumeratePhysicalDevices(instance, &count, hardware.data());
    for (const VkPhysicalDevice pd : hardware) {
      VkPhysicalDeviceProperties props {};
      VkPhysicalDeviceFeatures feat {};
      vkGetPhysicalDeviceProperties(pd, &props);
      vkGetPhysicalDeviceFeatures(pd, &feat);
      const bool is_gpu = props.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        | VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU
        | VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
      const bool is_cpu = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU;
      std::string deviceType = "OTHER";
      if (is_gpu) deviceType = "GPU";
      if (is_cpu) deviceType = "CPU";
      print("> " + std::string(props.deviceName));
      std::cout << "  - device type : " << deviceType << std::endl;
      std::cout << "  - geometryShader : " << (feat.geometryShader == VK_TRUE) << std::endl;
    }
    NEWLINE;
    physicalDevice = hardware[0];
  }

  {
    print("QUEUE FAMILIES");
    uint32_t count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    queueFamilies.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    for (const auto & qf : queueFamilies) {
      std::cout << "  - queue count : " << qf.queueCount << std::endl;
      std::cout << "  - minImage d " << qf.minImageTransferGranularity.depth
          << " | w " << qf.minImageTransferGranularity.width
          << " | h " << qf.minImageTransferGranularity.height
          << std::endl;
      std::cout << "      - graphics : " << std::boolalpha << ((qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) > 0) << std::endl;
      std::cout << "      - compute : " << ((qf.queueFlags & VK_QUEUE_COMPUTE_BIT) > 0) << std::endl;
      std::cout << "      - transfer : " << ((qf.queueFlags & VK_QUEUE_TRANSFER_BIT) > 0) << std::endl;
      std::cout << "      - sparse binding : " << ((qf.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) > 0) << std::endl;
      std::cout << "      - protected : " << ((qf.queueFlags & VK_QUEUE_PROTECTED_BIT) > 0) << std::endl;
      std::cout << "      - video decode : " << ((qf.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) > 0) << std::endl;
    }
    NEWLINE;
    queueFamilyIndex = 0;
  }

  print("DESTROYING THE WORLD");
  // no device to destroy yet.
  vkDestroyInstance(instance, nullptr);
  return EXIT_SUCCESS;
}