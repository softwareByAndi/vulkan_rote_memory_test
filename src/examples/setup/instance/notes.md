- set glfw flag to include stuff for vulkan
- includes for vulkan, glfw, and c structures
- global variables
    - vulkan instance
    - list of glfw required extensions
    - list of vulkan available extensions
---

- init glfw
  - we won't be creating a window in this example
- list glfw required extensions
  - append khr portability enumeration extension name
- list vulkan available extensions

---

- create vulkan instance
  - define application info
  - define instance info
    - pass required extensions
    - for mac, pass khr enumerate portability flag
  - create instance
  - check success

---

- destroy instance
- terminate glfw

