#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>

VkInstance instance;
std::vector<const char *> glfwRequiredExtensions;
std::vector<VkExtensionProperties> availableExtensions{};

int main() {
  glfwInit();

  { /// GLFW EXTENSIONS
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions; // NOTE : this is a reference to an array that's managed by GLFW
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    glfwRequiredExtensions = std::vector<const char *>(glfwExtensions, glfwExtensions + glfwExtensionCount);
    glfwRequiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    std::cout << std::endl;
    if (glfwExtensions == nullptr) {
      std::cout << " !! GLFW not initialized" << std::endl;
    }
    std::cout << "GLFW REQUIRED EXTENSIONS:" << std::endl;
    for (auto ext: glfwRequiredExtensions) {
      std::cout << "  - " << ext << std::endl;
    }
  }
  { /// AVAILABLE EXTENSIONS
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    availableExtensions.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

    std::cout << std::endl << "VULKAN AVAILABLE EXTENSIONS:" << std::endl;
    for (const auto &extension: availableExtensions) {
      std::cout << "  - " << extension.extensionName << std::endl;
    }
  }
  { /// INSTANCE
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
    info.enabledExtensionCount = (uint32_t) glfwRequiredExtensions.size();
    info.ppEnabledExtensionNames = glfwRequiredExtensions.data();
    info.enabledLayerCount = 0;
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    VkResult result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
    std::cout << std::endl << "INSTANCE CREATED SUCCESSFULLY" << std::endl;
  }

  vkDestroyInstance(instance, nullptr);
  glfwTerminate();
  return 0;
}