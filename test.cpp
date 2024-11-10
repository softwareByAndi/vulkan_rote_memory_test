/*
 * notify glfw to use vulkan stuff
 * import vulkan, glfw, iostream, vector, string
 *
 * tasks
 * - get required extensions from glfw
 * - append required extensions for mac
 * - check available extensions to instance
 * - confirm all required extensions are available
 * - define application info
 * - define instance info
 *    - attach required extensions
 *    - attach mac required flags
 * - create instance
 * - generate window
 * - poll window while open
 * - terminate program on window close
 *    - destroy instance
 *    - destroy window
 */
#define DNL "\n\n"
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <string>

GLFWwindow * window = nullptr;
VkInstance instance;
std::vector<const char *> reqEXT;
auto macEXT = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
auto macFlag = VK_KHR_portability_enumeration;

int main() {
  glfwInit();

  {
    /// REQUIRED EXTENSIONS
    uint32_t count = 0;
    auto glfwExt = glfwGetRequiredInstanceExtensions(&count);
    reqEXT = std::vector<const char *>(glfwExt, glfwExt + count);
    reqEXT.push_back(macEXT);
  }

  {
    /// AVAILABLE VALIDATION LAYERS
    // TODO
  }

  {
    /// CREATE INSTANCE
    std::cout << "CREATING INSTANCE" << std::endl;
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
    info.flags |= VK_KHR_portability_enumeration;
    info.pApplicationInfo = &ai;
    info.enabledLayerCount = 0;
    info.ppEnabledLayerNames = nullptr;
    info.enabledExtensionCount = reqEXT.size();
    info.ppEnabledExtensionNames = reqEXT.data();

    VkResult result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      std::cout << "  - FAILURE" << DNL;
      throw std::runtime_error("failed to create instance.");
    }
    std::cout << "  - SUCCESS" << DNL;
  }

  {
    /// DEBUG MESSENGER
//    std::cout << "CREATING DEBUG MESSENGER" << std::endl;
    // TODO
  }

  std::cout << "DESTROYING THE WORLD" << std::endl;
  vkDestroyInstance(instance, nullptr);
  glfwTerminate();
  return EXIT_SUCCESS;
}