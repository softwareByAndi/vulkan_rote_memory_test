# description

## PROBLEM STATEMENT
I keep forgetting the how the previous steps were implemented while building the renderer, and it makes it impossible to figure out how to refactor the api to add new functionality.

## MINIMUM GOAL
retain enough basic information about each step to re-write a basic renderer from scratch using documentation and auto-complete *(in a supplemental assistance capacity)*.

## SUPER GOAL
retain the names of flags, functions, and types so that the render can be written from scratch without the use of documentation or auto-complete

## PLAN 
1. break the renderer into steps of ~50 - 100 lines of code
2.  start each session with a blank file 
	a. re-code the previous steps from scratch until you reach the current step
3. start memorizing the current step *- this will likely span 2 sessions - one to learn & implement the material, and another session to simplify and memorize it.*
	a. go through the vulkan tutorial for the current step and implement the step in this session's implementation
	b. once the code is working, clean and simplify the code so that the sections related to the current step are easily identified
	c. write & re-write the current step until it can be done by heart 
4. repeat steps 2 & 3 until each step has been completed and a full vulkan renderer can be written from scratch
	- the expected progress is one step every 3 sessions
		- one session learn & implement new step
		- one session clean and memorize step
		- one session to re-write code from scratch including new step (will probably take 3 tries)

*each session is expected to span ~1 hours*
```
40 tasks * 3 sessions/task = 120 sessions
1 session/day = 4 months
```

# notes and updates
- *CURRENT STEP (physical device)* - as far as I can tell, this is working. It's slow, but that's because I've re-written the entire program over a dozen times by now, and I can write previous steps with ease. As I'm progressing, I'm realizing how simple each of the steps actually are - when validations, and abstractions are ripped away, each step is really simple and easy to remember. 

# TASKS
- [x] CMakeLists
- [ ] drawing a triangle
	- [ ] setup
		- [x] base code
		- [x] instance
		- [x] validation layers
		- [x] physical  devices & queue families
		- [ ] logical device & queues
	- [ ] presentation
		- [ ] window surface
		- [ ] swap chain
		- [ ] image views
	- [ ] graphics pipeline basics
		- [ ] introduction
		- [ ] shader modules
		- [ ] fixed functions
		- [ ] render passes
		- [ ] conclusion
	- [ ] drawing
		- [ ] frame buffers
		- [ ] command buffers
		- [ ] rendering and presentation
		- [ ] frames in flight
	- [ ] swap chain recreation

*CURRENT PROGRESS END*

- [ ] vertex buffers
	- [ ] vertex input description
	- [ ] vertex buffer creation
	- [ ] staging buffer
	- [ ] index buffer
- [ ] uniform buffers
	- [ ] descriptor layout and buffer
	- [ ] descriptor pool and sets
- [ ] texture mapping
	- [ ] images
	- [ ] image view and sampler
	- [ ] combined image sampler
- [ ] depth buffering
- [ ] loading models
- [ ] generating mipmaps
- [ ] multisampling
- [ ] compute shader
- [ ] FAQ
