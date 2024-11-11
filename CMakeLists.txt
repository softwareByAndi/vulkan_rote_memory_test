# set cmake version
# set cmake cXX version
# set project name
cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
project(vulkan_rote_memory_test)

# find packages
find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm)

# list global includes dirs
# list global link dirs
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Vulkan_INCLUDE_DIRS}
        "/opt/homebrew/include"
)
link_libraries(
        Vulkan::Vulkan
        "/opt/homebrew/lib/libglfw.3.3.dylib"
)

# add primary executables
add_executable(main main.cpp)
add_executable(test test.cpp)

# programmatically add example executables
foreach (NAME
        window
        instance
        validation_layers
        physical_device
)
    add_executable(
            "setup_${NAME}"
            "${CMAKE_CURRENT_SOURCE_DIR}/src/examples/setup/${NAME}/run.cpp"
    )
endforeach ()