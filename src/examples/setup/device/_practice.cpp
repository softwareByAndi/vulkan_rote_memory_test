/**
 * PRACTICE - VIRTUAL DEVICE
 *
 * TASKS
 * - select a physical gpu
 *     - save device properties
 *     - save device features
 *     - get / save device queue families
 *     - select a queue
 * - create a virtual device
 */

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <string>
#include <iostream>
#include <vector>

#define LOG_SUCCESS std::cout << "  - SUCCESS" << std::endl << std::endl
#define LOG_FAILURE std::cout << "  - FAILURE" << std::endl << std::endl
#define FAIL(x) LOG_FAILURE; throw std::runtime_error(x)
#define NEWLINE std::cout << std::endl
#define print(x) std::cout << x << std::endl
#define list(x) std::cout << "  - " << x << std::endl
#define DNL std::endl << std::endl;

#define macEXT VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
#define macFLAG VK_KHR_portability_enumeration
VkInstance instance;
std::vector<const char*> instanceEXT = {
  macEXT
};
std::vector<const char*> instanceLAY = {
  "VK_LAYER_KHRONOS_validation"
};

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
    print("SELECTING PHYSICAL DEVICE");
  }

  {
    print("CREATING VIRTUAL DEVICE");

  }


  print("DESTROYING THE WORLD");
  vkDestroyInstance(instance, nullptr);
  return EXIT_SUCCESS;
}