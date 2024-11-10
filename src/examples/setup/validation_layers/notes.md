notify glfw to use vulkan stuff
import vulkan, glfw, iostream, vector, cstring

tasks - INSTANCE
- get required extensions from glfw
- append required extensions for mac
- check available extensions to instance
- confirm all required extensions are available
- define application info
- define instance info
  - attach required extensions
  - attach mac required flags
- create instance
- generate window
- poll window while open
- terminate program on window close
  - destroy instance
  - destroy window

tasks - VALIDATION
- debug messenger
- instance layers
  - khronos validation
- define callbacks
  - debug - print message

- add required extensions for validation
- check available layers
- confirm requested layers are available
- pass layers to instance

- define debug messenger info
  - pass severity flags
  - pass message type flags
  - pass callback
- create debug messenger
  - query for create func

tasks - DESTROY
- destroy debug messenger
  - query for destroy func

tasks - OTHER
- factor vkResult check
- factor check missing required v.s. available
