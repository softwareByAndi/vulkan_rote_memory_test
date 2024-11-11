#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>

#define LOG_SUCCESS std::cout << "  - SUCCESS" << std::endl << std::endl
#define LOG_FAILURE std::cout << "  - FAILURE" << std::endl << std::endl
#define NEWLINE std::cout << std::endl
#define print(x) std::cout << x << std::endl
#define error(x) std::cout << x << std::endl
#define list(x) std::cout << "  - " << x << std::endl

#define macEXT VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
#define macFLAG VK_KHR_portability_enumeration
VkInstance instance;
std::vector<const char*> requiredExtensions = { macEXT };

int main() {
  glfwInit();

  {
    /// GLFW EXTENSIONS
    uint32_t count = 0;
    const char **glfwExtensions; // NOTE : this is a reference to an array that's managed by GLFW
    glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    if (glfwExtensions == nullptr) {
      error(" !! GLFW not initialized");
    } else {
      for (auto i = 0; i < count; i++) {
        requiredExtensions.push_back(glfwExtensions[i]);
      }
    }
  }

  {
    print("REQUIRED EXTENSIONS");
    for (auto ext: requiredExtensions) {
      list(ext);
    }
    NEWLINE;
  }

  {
    print("AVAILABLE EXTENSIONS");
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());
    for (const auto &extension: availableExtensions) {
      list(extension.extensionName);
    }
    NEWLINE;
  }

  {
    print("CREATING INSTANCE");
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "test";
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
    appInfo.pEngineName = "no engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    info.pApplicationInfo = &appInfo;
    info.enabledExtensionCount = (uint32_t) requiredExtensions.size();
    info.ppEnabledExtensionNames = requiredExtensions.data();
    info.enabledLayerCount = 0;
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    VkResult result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      LOG_FAILURE;
      throw std::runtime_error("failed to create instance!");
    }
    LOG_SUCCESS;
  }

  print("DESTROYING THE WORLD");
  vkDestroyInstance(instance, nullptr);
  glfwTerminate();
  return 0;
}