# image count
min 2, usually 3

# format
standard color-space and format combo is a nonlinear 8-bit BGRA
- `VK_COLOR_SPACE_SRGB_NONLINEAR_KHR`
- `VK_FORMAT_B8G8R8A8_SRGB`

# present mode
`FIFO` is pretty standard. There are other options.
- do some research for specific use cases

# swap extent
width and height of the image - usually the same size as the window, unless on a high DPI machine.
- get the window size using `glfwGetFramebufferSize`
- clamp that size to within the min & max of the surface extent

# create swapchain
- `VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT`
- `VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR`
- `info.clipped = VK_TRUE`;

### Sharing Mode:
- `EXCLUSIVE` is easiest. No need to manage resources between queues.
- `CONCURRENT` allows queues to use resources from other queues
  - e.g. if `presentQueue` is different from `graphicsQueue`
  - NOTE
    - access permissions need to be defined on swap creation
    - resources need to be managed - more on that when I get around to implementing
``` c
std::vector<uint32_t> queueFamilyIndices{ graphicsQFIndex, presetQFIndex };
info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
info.queueFamilyIndexCount = queueFamilyIndices.size();
info.pQueueFamilyIndices = queueFamilyIndices.data();
```


# get images
`vkGetSwapchainImagesKHR`
