#pragma once
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <lib/snippets/io_macros.h>
#include "lib/snippets/useful_functions.h"

namespace IO {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
  const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
  void*                                            pUserData
) {
  std::cout << GRAY << "DEBUG - ";

  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    std::cout << GRAY << "VERBOSE - ";
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    std::cout << BLUE << "INFO - ";
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    std::cout << YELLOW << "WARNING - ";
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    std::cerr << RED << "ERROR - ";

  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    std::cout << "GENERAL - ";
  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
    std::cout << "VALIDATION - ";
  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
    std::cout << "PERFORMANCE - ";
  if (messageTypes & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT)
    std::cout << "ADDRESS BINDING - ";

  std::cout << pCallbackData->pMessage << RESET << std::endl;
  return VK_FALSE;
}

template <typename T>
VkResult checkVkEnumeration(
  const std::vector<T> &available,
  const std::vector<const char *> &required
) {
  bool missingRequired = false;
  for (const auto& _name : required) {
    if (UF::includes(available, _name)) {
      list_blue(_name);
    }
    else {
      missingRequired = true;
      list_red(_name);
    }
  }
  if (missingRequired) {
    return VK_INCOMPLETE;
  }
  return VK_SUCCESS;
}

inline VkResult checkInstanceExtensions(
  const std::vector<const char *> &requiredExtensions
) {
  uint32_t count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(count);
  vkEnumerateInstanceExtensionProperties(nullptr, &count, availableExtensions.data());
  return checkVkEnumeration<VkExtensionProperties>(availableExtensions, requiredExtensions);
}
inline VkResult checkInstanceLayers(
  const std::vector<const char *> &requiredLayers
) {
  uint32_t count = 0;
  vkEnumerateInstanceLayerProperties(&count, nullptr);
  std::vector<VkLayerProperties> availableLayers(count);
  vkEnumerateInstanceLayerProperties(&count, availableLayers.data());
  return checkVkEnumeration<VkLayerProperties>(availableLayers, requiredLayers);
}
inline VkResult checkDeviceExtensions(
  const VkPhysicalDevice &physicalDevice,
  const std::vector<const char *> &requiredExtensions
) {
  uint32_t count = 0;
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, nullptr);
  std::vector<VkExtensionProperties> availableExtensions(count);
  vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &count, availableExtensions.data());
  return checkVkEnumeration<VkExtensionProperties>(availableExtensions, requiredExtensions);
}

}