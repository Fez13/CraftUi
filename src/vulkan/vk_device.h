#pragma once
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

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

  VkDevice get_device() const { return m_device; }
  void create_fence(VkFenceCreateFlags flags);

  /*
    @brief Creates a one time use command buffer, it will be deleted after being
    submitted.
  */
  VkCommandBuffer
  create_one_time_use_command_buffer(const VkCommandBufferLevel &bufferLevel,
                                     const std::string &pool_name);

  /*
    @brief Creates one or more one time use command buffer, they will be deleted
    after being submitted.
  */
  VkCommandBuffer create_command_buffers(const VkCommandBufferLevel bufferLevel,
                                         const uint32_t bufferCount,
                                         const VkCommandPool pool) const;

  /*
    @brief Ends and submits all command buffer in the array.
    @warning "pool_name" must be the same used to create the command buffer, and
    count must not exceed the count of buffers.
  */
  void submit_command_buffer(VkCommandBuffer *cmd, const uint32_t count,
                             const std::string &pool_name,
                             const uint32_t semaphores_count = 0,
                             const VkSemaphore *semaphores = nullptr);
  /*
    @brief Waits for a command pool to finish.
  */
  void wait_to_finish(const std::string pool_name);

  void free();

  /*
    @brief Returns all used queues.
  */
  std::vector<uint32_t> get_queues_indices() const {
    std::vector<uint32_t> queues;
    for (const auto &[name, queue] : m_queues)
      queues.push_back(queue.queue_index);
    return queues;
  }

  queue get_queue(const std::string name) {
    ASSERT(!m_queues.contains(name),
           "Error, device doesn't contain queue: " + name + ".",
           TEXT_COLOR_ERROR);
    return m_queues[name];
  }
  
  /*
  @brief Will create a new queue
  */
  void add_queue(const queue_info info);

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

  /*
    @brief Will return the device named as its parameter
    @warning If there isn't a device with such a name, it will crash.
    in order to check if there is a device with a name the fuction contains
    must be used.
  */
  inline vk_device *get_device(const std::string &name) {
    ASSERT(!m_devices.contains(name), "There isn't any device named :" + name,
           TEXT_COLOR_ERROR);
    return &m_devices[name];
  }

  /*
    @brief Will create a device based of the device constructor.
    @warning If there is a device named as the device tried to be created it
    will crash.
  */
  inline void create(const vkc_device_create_data &data) {
    ASSERT(m_devices.contains(data.device_name),
           "There is already a device named " + data.device_name,
           TEXT_COLOR_ERROR);
    m_devices[data.device_name] = vk_device(data);
  }

  /*
    @brief Returns true if there is a device with the input name.
  */
  inline bool contains(const std::string &name) {
    return m_devices.contains(name);
  }

  /*
    @brief Returns all used families.
  */
  std::vector<uint32_t> get_all_used_families() {
    std::vector<uint32_t> used;

    for (const auto &[name, device] : m_devices) {
      const std::vector<uint32_t> device_queues = device.get_queues_indices();
      for (const uint32_t &queue : device_queues)
        used.push_back(queue);
    }
    sort(used.begin(), used.end());
    used.erase(unique(used.begin(), used.end()), used.end());
    return used;
  }

private:
  std::unordered_map<std::string, vk_device> m_devices = {};
  static vk_device_manager s_device_manager;
};

} // namespace cui::vulkan
