#pragma once
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "vk_graphic_device.h"
#include "vk_utils.h"

namespace cui::vulkan {

struct queue {
  VkQueue queue;
  uint32_t queue_index;
  VkCommandPool command_pool;
};

class vk_device {
public:
  vk_device() = default;
  vk_device(const vkc_device_create_data &data);

  VkDevice get_device() { return m_device; }
  void create_fence(VkFenceCreateFlags flags);

  VkCommandBuffer
  create_one_time_use_command_buffer(const VkCommandBufferLevel &bufferLevel,
                                     const std::string &pool_name);

  VkCommandBuffer create_command_buffers(const VkCommandBufferLevel bufferLevel,
                                         const uint32_t bufferCount,
                                         const VkCommandPool pool) const;
  
  void submit_command_buffer(VkCommandBuffer *cmd,const uint32_t count, const std::string& pool_name);
  
  void wait_to_finish(const std::string pool_name) ;
  
  void free();

private:
  std::array<float, 3> m_queue_priorities;
  std::unordered_map<std::string, queue> m_queues;
  std::string m_name;
  VkDevice m_device;
  VkFence m_fence = VK_NULL_HANDLE;
};

struct vk_device_manager {
public:
  static vk_device_manager &get() { return s_device_manager; }

  inline vk_device *get_device(const std::string &name) {
    ASSERT(m_devices.contains(name), "There isn't any device named :" + name,
           TEXT_COLOR_ERROR);
    return &m_devices[name];
  }

  inline void create(const vkc_device_create_data &data) {
    ASSERT(!m_devices.contains(data.device_name),
           "There is already a device named " + data.device_name,
           TEXT_COLOR_ERROR);
    m_devices[data.device_name] = vk_device(data);
  }

private:
  std::unordered_map<std::string, vk_device> m_devices = {};
  static vk_device_manager s_device_manager;
};

} // namespace cui::vulkan
