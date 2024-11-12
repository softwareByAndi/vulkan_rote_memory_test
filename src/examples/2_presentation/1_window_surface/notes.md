# TLDR - Quick Ref
## MAC M1 - Metal
- native surface code for metal is found in `<vulkan/vulkan_metal.h>`
- tell vulkan to enable metal surface using `#define VK_USE_PLATFORM_METAL_EXT`
- use this function to create a metal surface `vkCreateMetalSurfaceEXT`

# problem statement
mac M1 is using metal surface, and KHR_display extension isn't supported (according to `vkEnumerateInstanceExtensionProperties`). 

# conclusion
So as far as I understand it, a native metal surface needs to be enabled instead of a generic surface.

# documentation
- metal surface https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap34.html#platformCreateSurface_metal
- WSI headers https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap57.html#boilerplate-wsi-header

- full WSI docs - I still need to go through this - https://registry.khronos.org/vulkan/specs/1.3-extensions/html/chap34.html

