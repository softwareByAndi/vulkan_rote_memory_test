# set cmake version
# set cmake cXX version
# set project name
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
project(vulkan_rote_memory_test)

# find packages
find_package(Vulkan REQUIRED)
find_package(glfw3 3.4 REQUIRED)
find_package(glm)
find_program(GLSL_VALIDATOR glslangValidator
    HINTS
        /usr/bin
        /usr/local/bin
        $ENV{VULKAN_SDK}/Bin/
        $ENV{VULKAN_SDK}/Bin32/
)

# list global includes dirs
# list global link dirs
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Vulkan_INCLUDE_DIRS}
        "/opt/homebrew/include"
)
link_libraries(
        Vulkan::Vulkan
        "/opt/homebrew/lib/libglfw.3.4.dylib"
)


## find all the shader files under the shaders folder
file(GLOB_RECURSE GLSL_SOURCE_FILES
        "${PROJECT_SOURCE_DIR}/shaders/*.frag"
        "${PROJECT_SOURCE_DIR}/shaders/*.vert"
        "${PROJECT_SOURCE_DIR}/shaders/*.comp"
        "${PROJECT_SOURCE_DIR}/shaders/**/*.frag"
        "${PROJECT_SOURCE_DIR}/shaders/**/*.vert"
        "${PROJECT_SOURCE_DIR}/shaders/**/*.comp"
)

message(STATUS ${GLSL_VALIDATOR})

## iterate each shader
foreach(GLSL ${GLSL_SOURCE_FILES})
    get_filename_component(FILE_NAME ${GLSL} NAME)
#    message(STATUS "BUILDING SHADER  - ${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
    set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
#    message(STATUS ${GLSL})
#    message(STATUS ${SPIRV})
    ## execute glslang command to compile that specific shader
    add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            COMMENT "Compiling shader: ${FILE_NAME}"
    )
    list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(Shaders DEPENDS ${SPIRV_BINARY_FILES})

# add primary executables
add_executable(main main.cpp)
add_executable(test test.cpp)

add_dependencies(main Shaders)

# programmatically add example executables
foreach (NAME
        01_window
        02_instance
        03_validation_layers
        04_physical_device
        05_device
        06_window_surface
        07_swapchain
        08_image_views
        09_shaders
        10_render_pass
        11_pipeline
#        12_framebuffer
#        13_command_pool
#        14_sync
        15_draw
)
    add_executable(
            "1_triangle_${NAME}"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/examples/1_triangle/${NAME}/run.cpp"
    )
endforeach ()