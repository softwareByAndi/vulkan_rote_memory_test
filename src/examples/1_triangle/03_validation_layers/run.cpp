#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include "lib/snippets/io_macros.h";

VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;
std::vector<const char *> requiredInstanceExtensions = {
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME, // MAC
  VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};
std::vector<VkExtensionProperties> availableInstanceExtensions;
std::vector<VkLayerProperties> availableInstanceLayers;
const std::vector<const char *> requestedInstanceValidationLayers = {
  "VK_LAYER_KHRONOS_validation",
//  "VK_LAYER_LUNARG_api_dump",
//  "VK_LAYER_KHRONOS_profiles",
//  "VK_LAYER_KHRONOS_synchronization2",
//  "VK_LAYER_KHRONOS_shader_object",
};

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
  SECTION("CREATING INSTANCE") {
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
    info.enabledLayerCount = static_cast<uint32_t>(requestedInstanceValidationLayers.size());
    info.ppEnabledLayerNames = requestedInstanceValidationLayers.data();

    const VkResult result = vkCreateInstance(
      &info, nullptr, &instance
    );
    if (result != VK_SUCCESS) {
      FAIL("failed to create instance");
    }
    LOG_SUCCESS;
  }
  SECTION("CREATING DEBUG MESSENGER") {
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
    const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance,"vkCreateDebugUtilsMessengerEXT")
    );
    if (func != nullptr) {
      const VkResult result = func(
        instance,
        &info,
        nullptr,
        &debugMessenger
      );
      if (result != VK_SUCCESS) {
        FAIL("failed to create instance");
      }
    }
    else {
      FAIL("  - ERROR : unable to find procAddr for vkCreateDebugUtilsMessengerEXT");
    }
    LOG_SUCCESS;
  }

  SECTION("DESTROYING THE WORLD") {
    {
      const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")
      );
      if (func != nullptr) func(instance, debugMessenger, nullptr);
      else error("  ! ERROR : unable to destroy debugMessenger");
    }
    vkDestroyInstance(instance, nullptr);
    LOG_SUCCESS;
  }
  return EXIT_SUCCESS;
}