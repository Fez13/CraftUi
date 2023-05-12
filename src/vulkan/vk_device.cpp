#include "vk_device.h"

namespace cui::vulkan {

device_manager device_manager::s_device_manager;

vk_device::vk_device(const vkc_device_create_data &data) {
  // Creates 3 queues graphic, compute and transfer
  m_name = data.device_name;
  // TODO:
  m_queue_priorities[0] = 1;
  m_queue_priorities[1] = 1;
  m_queue_priorities[2] = 1;

  std::vector<VkDeviceQueueCreateInfo> queue_create_info;

  if (data.graphic_family != UINT32_MAX) {
    queue_create_info.emplace_back(vkcDeviceQueueCreateInfo(
        0, 1, &m_queue_priorities[0], data.graphic_family));
    m_queues["GRAPHIC"] =
        queue{VK_NULL_HANDLE, data.graphic_family, VK_NULL_HANDLE};
  }
  if (data.compute_family != UINT32_MAX) {
    queue_create_info.emplace_back(vkcDeviceQueueCreateInfo(
        0, 1, &m_queue_priorities[1], data.compute_family));
    m_queues["COMPUTE"] =
        queue{VK_NULL_HANDLE, data.compute_family, VK_NULL_HANDLE};
  }
  if (data.transfer_family != UINT32_MAX) {
    queue_create_info.emplace_back(vkcDeviceQueueCreateInfo(
        0, 1, &m_queue_priorities[2], data.transfer_family));
    m_queues["TRANSFER"] =
        queue{VK_NULL_HANDLE, data.transfer_family, VK_NULL_HANDLE};
  }
  VkDeviceCreateInfo device_create_info =
      vkcDeviceCreateInfo(queue_create_info, VK_NULL_HANDLE,
                          data.gpu_extensions, data.features_chain);
  VK_CHECK(vkCreateDevice(vk_graphic_device::get().get_device(),
                          &device_create_info, nullptr, &m_device));

  // Creates command pools and queues
  for (auto &[name, obj] : m_queues) {

    // Locate queue
    vkGetDeviceQueue(m_device, obj.queue_index, 0, &obj.queue);

    // Create pool
    VkCommandPoolCreateInfo pool_create_info =
        vkcCommandPoolCreateInfo(obj.queue_index);
    VK_CHECK(vkCreateCommandPool(m_device, &pool_create_info, nullptr,
                                 &obj.command_pool) != VK_SUCCESS);
  }
}
void vk_device::free() {
  for (auto &[name, obj] : m_queues)
    vkDestroyCommandPool(m_device, obj.command_pool, nullptr);
  vkDestroyFence(m_device, fence, nullptr);
  vkDestroyDevice(m_device, nullptr);
}
} // namespace cui::vulkan
