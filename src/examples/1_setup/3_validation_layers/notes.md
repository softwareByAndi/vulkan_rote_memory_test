NOTES
The debugMessenger is optional - it allows you to tune which messages will be printed.


tasks - VALIDATION
- instance layers
  - khronos validation
- check available layers
- confirm requested layers are available
- pass layers to instance

tasks - DEBUG CALLBACK
- define debug messenger variable
- add required extensions for debug
- define callbacks
  - debug - print message
  - should be a static function
- define debug messenger info
  - pass severity flags
  - pass message type flags
  - pass callback
- create debug messenger
  - query for create func
- destroy debug messenger
  - query for destroy func

tasks - OTHER
- factor vkResult check
- factor check missing required v.s. available

