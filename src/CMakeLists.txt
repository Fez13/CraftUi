cmake_minimum_required(VERSION 3.22.1)
project(CraftUI)

set(CMAKE_CXX_STANDARD 23)

find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)

INCLUDE_DIRECTORIES(vendor/glm)
LINK_DIRECTORIES(vendor/glm)

add_executable(CraftUI main.cpp)

target_link_libraries(CraftUI Vulkan::Vulkan)
target_link_libraries(CraftUI glfw)



