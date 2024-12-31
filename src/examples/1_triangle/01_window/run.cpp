#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "lib/snippets/io_macros.h";

void glfwErrorCallback(int code, const char* description) {
  std::cerr << "GLFW ERROR " << code << ": " << description << std::endl;
}

int main() {
  glfwInit();
  glfwSetErrorCallback(glfwErrorCallback);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  GLFWwindow * window = glfwCreateWindow(800, 600, "window title", nullptr, nullptr);

  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  std::vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

  NEWLINE;
  std::cout << extensionCount << " extensions available" << std::endl;
  for (const auto& ext : extensions) {
    list_gray(ext.extensionName);
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  LOG_SUCCESS;
  return 0;
}