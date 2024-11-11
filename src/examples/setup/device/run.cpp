#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <vector>
#include <optional>

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
std::vector<const char*> reqEXT = {
  macEXT
};
std::vector<const char*> reqLAY = {
  "VK_LAYER_KHRONOS_validation"
};

VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
VkPhysicalDeviceProperties properties;
VkPhysicalDeviceFeatures features;
std::vector<VkQueueFamilyProperties> queueFamilies{};
uint32_t queueFamilyIndex = 0;


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
    info.enabledLayerCount = reqLAY.size();
    info.ppEnabledLayerNames = reqLAY.data();
    info.enabledExtensionCount = reqEXT.size();
    info.ppEnabledExtensionNames = reqEXT.data();

    auto result = vkCreateInstance(&info, nullptr, &instance);
    if (result != VK_SUCCESS) {
      FAIL("failed to create instance");
    }
    LOG_SUCCESS;
  }

  {
    print("SELECTING PHYSICAL DEVICE");
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> devices(count);
    vkEnumeratePhysicalDevices(instance, &count, devices.data());
    if (devices.empty() || devices[0] == VK_NULL_HANDLE) {
      FAIL("failed to find a device");
    }
    physicalDevice = devices[0];
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    const auto GPUFlags = VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU | VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU | VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
    const bool isGPU = (properties.deviceType & GPUFlags) > 0;
    if (!isGPU) {
      FAIL("selected device is not a GPU");
    }
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    queueFamilies.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, queueFamilies.data());
    if (queueFamilies.empty()) {
      FAIL("selected device has no queues...");
    }
    queueFamilyIndex = 0;
    LOG_SUCCESS;
  }

  {
    print("CREATING VIRTUAL DEVICE");

  }


  print("DESTROYING THE WORLD");
  vkDestroyInstance(instance, nullptr);
  return EXIT_SUCCESS;
}