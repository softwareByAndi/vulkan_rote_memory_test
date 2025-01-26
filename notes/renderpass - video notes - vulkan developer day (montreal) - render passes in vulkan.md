
- https://www.youtube.com/watch?v=yeKxsmlvvus
# todo
- look into dynamic rendering (vulkan 1.3)
- gpu debugger - e.g. renderdoc
- gpu profilers - useful for measurements (renderdoc isn't a profiler)
# notes
- `pNext` is a pointer that allows you to extend the struct. *- probably to include extra information for use in extensions*
- `flags` is reserved - there are no flags.
- `renderpasses` are a collection of `imageViewAttachment` locations (0-n) and you need to describe what it is you would like to attach

image view attachments
	- `alias` aliasing one attachment as another
	- `sampleCount` how many samples are included in the attachment
	- `LOAD_OP`
		- `LOAD` = preserve contents
		- `CLEAR` = clear to a known good value (e.g. for depth buffers)
		- `DONT_CARE` = use some sort of fast clear on the buffer. don't care what the value is.
		- takes place sometime between the beginning of the renderpass instance, and the first use of the actual input attachment
	- `STORE_OP`
		- `STORE` = save and store results
		- `DONT_CARE` = discard results / don't ever show up outside of the subpass
		- takes place sometime between the last time that it was used and the end of the subpass, or as a cleanup op at the end of the renderpass instance

## subpass 
- attachments
- aspect ratio
### attachments
> *"think of these as members of a team"*
- input attachments
- color attachments
- depth stencil attachments
- preserve attachments

### dependencies
- `srcSubpass` = who am I dependent on?
- `dstSubpass` = who I am
- `srcStageMask` 
- `dstStageMask`
- `srcAccessMask` 
- `dstAccessMask`
- `dependencyFlags`

## framebuffers
`framebuffers` represent a collection of specific memory attachment sthat a render pass instance uses
pass in 
- renderpass
- attachment counts
- size

## image views
- `subResourseRange` = what slice out of the whole image makes up this image view? (whole image might be a 3D image, etc...)
## image
### transient images
`VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT` = lazy allocate memory for transient images - implies that you never need to access the results from an image after the renderpass exits.
	- cannot `LOAD` transient images - can only `VK_ATTACHMENT_LOAD_OP_CLEAR` or `VK_ATTACHMENT_LOAD_OP_DONT_CARE`
	- cannot `STORE` transient images - can only `VK_ATTACHMENT_STORE_OP_DONT_CARE` 

# build render pass
- `VkCmdBeginRenderPass(...)`

# subpass examples
## simple 1999 rendering style
- Z Only subpass
- FP16 subpass
- "Tonemap" subpass (manual exposure control)
![[Pasted image 20250125191841.png]]
![[Pasted image 20250125191855.png]]
![[Pasted image 20250125192251.png]]
