#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <GLFW/glfw3.h>

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYERS = false;
#else
const bool ENABLE_VALIDATION_LAYERS = true;
#endif

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
std::vector<const char *> requiredInstanceExtensions;
std::vector<VkExtensionProperties> availableInstanceExtensions;
std::vector<VkLayerProperties> availableInstanceLayers;
const std::vector<const char *> requestedInstanceValidationLayers = {
  "VK_LAYER_KHRONOS_validation",
//  "VK_LAYER_LUNARG_api_dump",
//  "VK_LAYER_KHRONOS_profiles",
//  "VK_LAYER_KHRONOS_synchronization2",
//  "VK_LAYER_KHRONOS_shader_object",
//  "TEST_fake_validation_layer",
};


void handleVkResult(VkResult result) {
  if (result != VK_SUCCESS) {
    std::cerr << "  - ERROR : VkResult = " << result << std::endl;
    throw std::runtime_error("VkResult was not successful");
  } else {
    std::cout << "  - SUCCESS" << std::endl;
  }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData
) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

int main() {
  glfwInit();
  /// REQUIRED EXTENSIONS
  {
    std::cout << "REQUIRED EXTENSIONS (INSTANCE)" << std::endl;
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    requiredInstanceExtensions = std::vector<const char *>{
      glfwExtensions,
      glfwExtensions + glfwExtensionCount
    };
    requiredInstanceExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
    if (ENABLE_VALIDATION_LAYERS) {
      requiredInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    for (auto c: requiredInstanceExtensions) {
      std::cout << "  - " << c << std::endl;
    }
  }
  /// AVAILABLE INSTANCE EXTENSIONS
  {
    std::cout << std::endl << "VK AVAILABLE EXTENSIONS (INSTANCE)" << std::endl;
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(
      nullptr,
      &extensionCount,
      nullptr
    );
    availableInstanceExtensions.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(
      nullptr,
      &extensionCount,
      availableInstanceExtensions.data()
    );
    for (auto ext : availableInstanceExtensions) {
      std::cout << "  - " << ext.extensionName << std::endl;
    }
  }
  /// VALIDATION LAYERS
  if (ENABLE_VALIDATION_LAYERS) {
    std::cout << std::endl << "REQUESTED VALIDATION LAYERS (INSTANCE):" << std::endl;
    for (auto s : requestedInstanceValidationLayers) {
      std::cout << "  - " << s << std::endl;
    }
    std::cout << "AVAILABLE VALIDATION LAYERS (INSTANCE):" << std::endl;
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    availableInstanceLayers = std::vector<VkLayerProperties>(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableInstanceLayers.data());
    for (auto l : availableInstanceLayers) {
      std::cout << "  - " << l.layerName << std::endl;
    }
    std::cout << "CHECKING VALIDATION LAYERS (INSTANCE):" << std::endl;
    std::vector<const char *> missingLayers{};
    for (const char * layerName : requestedInstanceValidationLayers) {
      bool layerFound = false;
      for (const auto& layerProperties : availableInstanceLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }
      if (!layerFound) {
        missingLayers.emplace_back(layerName);
      }
    }
    if (missingLayers.empty()) {
      std::cout << "  - SUCCESS" << std::endl;
    }
    else {
      std::cout << "  MISSING LAYERS:" << std::endl;
      for (const char * s : missingLayers) {
        std::cerr << "    -  " << s << std::endl;
      }
      throw std::runtime_error("validation layers requested, but not available!");
    }
  }
  /// INSTANCE
  {
    std::cout << std::endl << "CREATING INSTANCE:" << std::endl;
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
    info.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
    info.ppEnabledExtensionNames = requiredInstanceExtensions.data();
    if (ENABLE_VALIDATION_LAYERS) {
      info.enabledLayerCount = static_cast<uint32_t>(requestedInstanceValidationLayers.size());
      info.ppEnabledLayerNames = requestedInstanceValidationLayers.data();
    }
    else {
      info.enabledLayerCount = 0;
      info.ppEnabledLayerNames = VK_NULL_HANDLE;
    }

    VkResult result = vkCreateInstance(
      &info, nullptr, &instance
    );
    handleVkResult(result);
  }
  /// DEBUG MESSENGER
  if (ENABLE_VALIDATION_LAYERS) {
    std::cout << std::endl << "CREATING DEBUG MESSENGER:" << std::endl;
    VkDebugUtilsMessengerCreateInfoEXT info{};
    info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    info.pNext = VK_NULL_HANDLE;
    info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    info.pfnUserCallback = debugCallback;
    info.pUserData = nullptr;
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
      instance,
      "vkCreateDebugUtilsMessengerEXT"
    );
    if (func != nullptr) {
      VkResult result = func(
        instance,
        &info,
        nullptr,
        &debugMessenger
      );
      handleVkResult(result);
    }
    else {
      std::cerr << "  - ERROR : unable to find procAddr for vkCreateDebugUtilsMessengerEXT" << std::endl;
      handleVkResult(VK_ERROR_EXTENSION_NOT_PRESENT);
    }
  }

  std::cout << std::endl << "DESTROYING THE WORLD" << std::endl;
  if (ENABLE_VALIDATION_LAYERS) {
    /// DESTROY DEBUG UTILS
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
      instance,
      "vkDestroyDebugUtilsMessengerEXT"
    );
    if (func != nullptr) func(instance, debugMessenger, nullptr);
    else std::cerr << "  ! ERROR : unable to destroy debugMessenger" << std::endl;
  }
  vkDestroyInstance(instance, nullptr);
  glfwTerminate();
  return EXIT_SUCCESS;
}