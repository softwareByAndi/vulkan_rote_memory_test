/**
 * PRACTICE - VALIDATION LAYERS
 *
 * TASKS
 * - add validation extension
 * - create a debugMessenger that wraps & prints all validation messages
 * - destroy debugMessenger
 */

#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>

#define LOG_SUCCESS std::cout << "  - SUCCESS" << std::endl << std::endl
#define LOG_FAILURE std::cerr << "  - FAILURE" << std::endl << std::endl
#define NEWLINE std::cout << std::endl
#define print(x) std::cout << x << std::endl
#define error(x) std::cerr << x << std::endl
#define list(x) std::cout << "  - " << x << std::endl

#define macEXT VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
#define macFLAG VK_KHR_portability_enumeration
VkInstance instance;
std::vector<const char *> requiredInstanceExtensions = {
  macEXT
};

int main() {
  {
    print("CREATING INSTANCE");
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = VK_NULL_HANDLE;
    appInfo.pApplicationName = "Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_MAKE_API_VERSION(3, 0, 0, 0);

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pNext = VK_NULL_HANDLE;
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    info.pApplicationInfo = &appInfo;
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = VK_NULL_HANDLE;
    info.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
    info.ppEnabledExtensionNames = requiredInstanceExtensions.data();

    VkResult result = vkCreateInstance(
      &info, nullptr, &instance
    );
    if (result != VK_SUCCESS) {
      LOG_FAILURE;
      throw std::runtime_error("failed to create instance");
    }
    LOG_SUCCESS;
  }

  {
    print("CREATING DEBUG MESSENGER");
  }

  print("DESTROYING THE WORLD");
  vkDestroyInstance(instance, nullptr);
  return EXIT_SUCCESS;
}