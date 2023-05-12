cmake_minimum_required(VERSION 3.22.1)
project(CraftUI)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

INCLUDE_DIRECTORIES(vendor/glm)
LINK_DIRECTORIES(vendor/glm)

add_executable(CraftUI main.cpp 
src/core/app.h src/core/app.cpp 
src/entities/entity.h 
src/renderer/renderer.h src/renderer/renderer.cpp 
src/vulkan/vk_buffer.h src/vulkan/vk_buffer.cpp 
src/vulkan/vk_graphic_device.h src/vulkan/vk_graphic_device.cpp 
src/vulkan/vk_instance.h src/vulkan/vk_instance.cpp 
src/vulkan/vulkan.h src/vulkan/vulkan.cpp 
src/vulkan/vk_utils.h src/vulkan/vk_utils.cpp 
src/vulkan/vk_device.h src/vulkan/vk_device.cpp 
src/vulkan_config/vulkan_config.hpp 
src/vendor/stb/stb.cpp src/vendor/stb/stb_image.h src/vendor/stb/stb_truetype.h 
src/vendor/tinyobjloader/tiny_obj_loader.h src/vendor/tinyobjloader/tiny_obj_loader.cpp)


target_link_libraries(CraftUI Vulkan::Vulkan)
target_link_libraries(CraftUI glfw)


