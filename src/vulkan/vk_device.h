#pragma once
#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include "vk_graphic_device.h"
#include "vk_utils.h"

namespace cui::vulkan {

struct queue{
  VkQueue queue;
  uint32_t queue_index;
  VkCommandPool command_pool;
};

class vk_device {
public:
  vk_device() = default;
  vk_device(const vkc_device_create_data &data);


  void free();
private:
  std::array<float, 3> m_queue_priorities;
  std::unordered_map<std::string, queue> m_queues;
  VkDevice m_device;
  std::string m_name;
};

struct device_manager {
public:
  inline vk_device *get(const std::string &name) {
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
  std::unordered_map<std::string, vk_device> m_devices;
  static device_manager s_device_manager;
};

} // namespace cui::vulkan
