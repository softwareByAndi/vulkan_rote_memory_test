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
  }

  {
    print("SELECTING PHYSICAL DEVICE");
  }

  {
    print("QUEUE FAMILIES");
  }


  print("DESTROYING THE WORLD");
  vkDestroyInstance(instance, nullptr);
  return EXIT_SUCCESS;
}