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

VkBufferCreateInfo vkcBufferCreateInfo(const VkSharingMode sharing,
                                       const size_t size,
                                       const VkBufferUsageFlags usage);

VkMemoryAllocateInfo vkcMemoryAllocateInfo(const uint32_t memory_index,
                                           const size_t size);

//
// Structures
//

using queue_info = std::pair<std::string, uint32_t>;

struct vkc_device_create_data {
  std::string device_name;
  std::vector<queue_info> queues;
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

uint32_t
find_memory(const VkMemoryPropertyFlags memory_properties,
           VkPhysicalDeviceMemoryProperties physical_device_memory_properties,
           const uint32_t memory_type = 1);

//
// Descriptor sets
//

VkDescriptorSetLayoutBinding vkcDescriptorSetLayoutBinding(
    const uint32_t binding, const uint32_t count, const VkDescriptorType type,
    const VkSampler *sampler, const VkShaderStageFlags stages);

VkDescriptorSetLayoutCreateInfo
vkcDescriptorSetLayoutCreateInfo(const uint32_t count,
                                 const VkDescriptorSetLayoutBinding *bindings);

VkDescriptorPoolCreateInfo
vkcDescriptorPoolCreateInfo(const uint32_t count,
                            const VkDescriptorPoolSize *pool_sizes);

VkDescriptorSetAllocateInfo
vkcDescriptorSetAllocateInfo(const VkDescriptorSetLayout *layout,
                             const VkDescriptorPool pool);

struct descriptor_binding {
  VkDescriptorType type;
  VkDescriptorSetLayoutBinding layout_binding;
  uint32_t count = 0;
};

//
// Images
//

struct vkc_image_create_data {
  glm::ivec2 size;
  VkFormat format;
  VkImageTiling tiling;
  VkImageUsageFlags usage;
  VkMemoryPropertyFlagBits properties;
  VkImage image;
  VkDeviceMemory image_memory;
  VkImageLayout image_layout;
};

VkImageCreateInfo
vkcImageCreateInfo(const vkc_image_create_data &data);

} // namespace cui::vulkan
