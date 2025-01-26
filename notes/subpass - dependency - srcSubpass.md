

*Ah, the joys of srcSubpass. It’s not just `VK_SUBPASS_EXTERNAL` all day long—although you probably wish it were. Here’s the list of options you have, explained in detail so you can pretend to understand Vulkan while secretly cursing it under your breath:*

# TL;DR
- `VK_SUBPASS_EXTERNAL`: sync with external processes *(before/after render pass)*
- Valid subpass index (0, 1, …): sync subpasses **within** a render pass.
	- `srcSubpass` <= `dstSubpass`
	- you can synchronize a subpass with itself

# Values
## 1. VK_SUBPASS_EXTERNAL
*The big bad outsider.* This means the source or destination of the dependency lies outside the render pass entirely.
### When to use it:
- Transitioning resources between a render pass and the rest of the pipeline (e.g., preparing an image for reading before rendering).
- Dealing with image layouts *(e.g., going from `VK_IMAGE_LAYOUT_PRESENT_SRC_KHR` to `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL`).*

## 2. Subpass Index (0, 1, 2, …)
The number corresponds to a specific subpass index within your render pass.
### When to use it:
Dependencies within the same render pass.

For example, if you have:
- Subpass 0: Producing a depth buffer.
- Subpass 1: Using that depth buffer for post-processing.
You’d set srcSubpass = 0 and dstSubpass = 1 to synchronize them.

# Rules and Constraints
## 1.	Subpass Index Range:
The value of srcSubpass must be `VK_SUBPASS_EXTERNAL` or a valid subpass index within the render pass *(from 0 up to subpassCount - 1)*
- Anything outside this range will cause Vulkan to laugh at your naïve optimism and return `VK_ERROR_INITIALIZATION_FAILED`.
## 2.	Same Render Pass Only:
Subpass dependencies using an index *(e.g., 0, 1)* only apply within the same render pass. For anything outside, you must resort to `VK_SUBPASS_EXTERNAL`.
## 3.	Self-Dependencies:
Want to synchronize a subpass with itself? Sure, go ahead:
``` cpp
dependency.srcSubpass = 0;
dependency.dstSubpass = 0;
```
Useful for multi-stage rendering, like using a color attachment as both input and output (input attachments FTW!).
## 4.	Order Matters:
When specifying `srcSubpass` and `dstSubpass`, `srcSubpass` must always come before or be the same as `dstSubpass` in the execution order.
- Trying to reverse time? Vulkan doesn’t like temporal paradoxes.

# Practical Example: Multi-Subpass Render Pass

Imagine you have two subpasses:
1.	Subpass 0: Renders a depth buffer.
2.	Subpass 1: Applies lighting using the depth buffer.

Dependency setup:
``` cpp
VkSubpassDependency dependency{};
dependency.srcSubpass = 0;  // Subpass 0 is the producer.
dependency.dstSubpass = 1;  // Subpass 1 is the consumer.

dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
dependency.dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
```


