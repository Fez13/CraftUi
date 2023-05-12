#pragma once

#include <cmath>
#include <cstring>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>

#include "../core/macros.h"
#include "../vendor/glm/glm.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace cui::vulkan {
struct deletion_queue {

public:
  void add_to_queue(std::function<void(VkDevice, VkPhysicalDevice)> fn) {
    queue.push_back(fn);
  }

private:
  std::vector<std::function<void(VkDevice, VkPhysicalDevice)>> queue;
};

//
// Initializers
//
VkApplicationInfo
vkcApplicationInfo(const char *app_name, const uint32_t app_version,
                   const uint32_t api_version,
                   const glm::ivec3 engine_version = glm::vec3(1, 0, 0));

VkInstanceCreateInfo
vkcInstanceCreateInfo(VkApplicationInfo &app_info,
                      const std::vector<const char *> &extensions,
                      const std::vector<const char *> &layers);

VkDeviceQueueCreateInfo vkcDeviceQueueCreateInfo(VkDeviceQueueCreateFlags flags,
                                                 const uint32_t queue_count,
                                                 float *queue_priority,
                                                 const uint32_t queue_index);

VkDeviceCreateInfo
vkcDeviceCreateInfo(const std::vector<VkDeviceQueueCreateInfo> &queue_info,
                    const VkPhysicalDeviceFeatures *features,
                    const std::vector<const char *> *extensions,
                    const void *pnext);

VkCommandPoolCreateInfo vkcCommandPoolCreateInfo(const uint32_t queue_index);
//
// Structures
//
struct vkc_device_create_data {
  std::string device_name;
  uint32_t graphic_family;
  uint32_t compute_family;
  uint32_t transfer_family;
  std::vector<const char *> *gpu_extensions;
  void *features_chain;
};

//
// Queues and devices
//
bool device_suitable(VkPhysicalDevice const &physical_device,
                     std::function<bool(VkPhysicalDeviceProperties &,
                                        VkPhysicalDeviceFeatures &)>
                         checks);

uint32_t getSuitableQueueFamily(
    VkPhysicalDevice physical_device,
    std::function<bool(const VkQueueFamilyProperties &fp)> checks);

//
// Memory functions
//

} // namespace cui::vulkan
