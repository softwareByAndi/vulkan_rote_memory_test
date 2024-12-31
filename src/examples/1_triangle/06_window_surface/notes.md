
# Window Surface
it's pretty simple...
- create window
- create surface
- confirm surface support
``` cpp
glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
window = glfwCreateWindow(800, 600, "Window Surface Example", nullptr, nullptr);

glfwCreateWindowSurface(instance, window, nullptr, &surface)

VkBool32 isSupported;
vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &isSupported);
```

# Surface Details for Swap chain
- Surface Capabilities
  - extent
  - image count
  - etc...
- Present Modes
  - `FIFO`
  - `MAILBOX`
  - etc...
- Formats
  - color space
  - format / data-structure
    - e.g. 
      - bits per color
      - color order
      - data type *( int v.s. float v.s sRGB, etc... )*
