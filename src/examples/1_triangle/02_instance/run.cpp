#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <iostream>

#include "lib/snippets/io_macros.h";
#include "lib/snippets/useful_functions.h"

VkInstance instance;
std::vector<const char*> requiredExtensions = {
  // "TEST",
  VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME // MAC
};

int main() {
  glfwInit();

  NEWLINE;

  SECTION("GLFW REQUIRED EXTENSIONS") {
    uint32_t count = 0;
    // NOTE : this is a reference to an array that's managed by GLFW
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&count);
    if (glfwExtensions == nullptr) {
      FAIL(" !! GLFW not initialized");
    }
    for (auto i = 0; i < count; i++) {
      requiredExtensions.push_back(glfwExtensions[i]);
    }

    count = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(count);
    vkEnumerateInstanceExtensionProperties(nullptr, &count, availableExtensions.data());

    for (const auto &[extensionName, _v]: availableExtensions) {
      if (UF::includes(requiredExtensions, extensionName)) list_blue(extensionName);
      else list_gray(extensionName);
    }
    bool extensionsNotFound = false;
    for (const auto &extensionName : requiredExtensions) {
      if (!UF::includes(availableExtensions, extensionName)) {
        extensionsNotFound = true;
        list_red(extensionName);
      }
    }
    if (extensionsNotFound) {
      FAIL("required extensions not found");
    }
    LOG_SUCCESS;
  }
  SECTION("CREATING INSTANCE") {
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
    info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    info.ppEnabledExtensionNames = requiredExtensions.data();
    info.enabledLayerCount = 0;
    info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    if (VK_SUCCESS != vkCreateInstance(&info, nullptr, &instance)) {
      LOG_FAILURE;
      throw std::runtime_error("failed to create instance!");
    }
    LOG_SUCCESS;
  }
  SECTION ("DESTROYING THE WORLD") {
    vkDestroyInstance(instance, nullptr);
    glfwTerminate();
    LOG_SUCCESS;
  }
  return 0;
}