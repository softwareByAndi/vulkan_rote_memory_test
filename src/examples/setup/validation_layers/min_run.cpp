/*copy this file to practice adding validation layers to the instance*/

#define DNL "\n\n"
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <string>

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
std::vector<const char *> reqEXT;
#define debugEXT VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#define macEXT VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
#define macFlag VK_KHR_portability_enumeration
std::vector<const char *> reqLAY = {
  "VK_LAYER_KHRONOS_validation"
};


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
  const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
  void*                                            pUserData
) {
  std::cout << "DEBUG CALLBACK : " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

int main() {
  glfwInit();

  {
    /// REQUIRED EXTENSIONS
    uint32_t count = 0;
    auto glfwExt = glfwGetRequiredInstanceExtensions(&count);
    reqEXT = std::vector<const char *>(glfwExt, glfwExt + count);
    reqEXT.emplace_back(macEXT);
    reqEXT.emplace_back(debugEXT);
  }

  {
    /// AVAILABLE VALIDATION LAYERS
    std::cout << "AVAILABLE VALIDATION LAYERS" << std::endl;
    uint32_t count = 0;
    vkEnumerateInstanceLayerProperties(&count, nullptr);
    std::vector<VkLayerProperties> layers(count);
    vkEnumerateInstanceLayerProperties(&count, layers.data());
    for (const auto & layer : layers) {
      std::cout << "  - " << layer.layerName << std::endl;
    }
    std::cout << std::endl;
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
    info.flags |= macFlag;
    info.pApplicationInfo = &ai;
    info.enabledLayerCount = reqLAY.size();
    info.ppEnabledLayerNames = reqLAY.data();
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
    std::cout << "CREATING DEBUG MESSENGER" << std::endl;
    VkDebugUtilsMessengerCreateInfoEXT info {};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.pNext = VK_NULL_HANDLE;
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
    info.pfnUserCallback = debugCallback;
    info.pUserData = nullptr;

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
      instance,
      "vkCreateDebugUtilsMessengerEXT"
    );
    VkResult result = func(instance, &info, nullptr, &debugMessenger);
    if (result != VK_SUCCESS) {
      std::cout << "  - FAILURE" << DNL;
      throw std::runtime_error("failed to create debug messenger");
    }
    std::cout << "  - SUCCESS" << DNL;

  }

  std::cout << "DESTROYING THE WORLD" << std::endl;
  {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
      instance,
      "vkDestroyDebugUtilsMessengerEXT"
    );
    func(instance, debugMessenger, nullptr);
  }
  vkDestroyInstance(instance, nullptr);
  glfwTerminate();
  return EXIT_SUCCESS;
}