
- also see [[renderpass - stages - painter analogy]]

"subpass `0` did something in the `COLOR_ATTACHMENT` stage, and subpass `1` needs the results of that operation for use in the `FRAGMENT_SHADER` stage."
- the thing that subpass 0 did was `WRITE`
- and the thing that subpass 1 needs to do is `READ` 
``` cpp
VkSubpassDependency dependency = {};
dependency.srcSubpass = 0;  // First subpass
dependency.dstSubpass = 1;  // Second subpass
dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
```
In a subpass dependency, the access masks describe a producer-consumer relationship between two specific operations:
- `srcAccessMask` describes what kind of access operation needs to finish in the source subpass
- `dstAccessMask` describes what kind of access operation needs to wait in the destination subpass
So when we write:
``` cpp
srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
```
We're saying: "Wait until the source subpass finishes its `COLOR_ATTACHMENT_WRITE` operations before the destination subpass starts its `INPUT_ATTACHMENT_READ` operations." 
