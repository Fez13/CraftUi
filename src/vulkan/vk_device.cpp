#include "vk_device.h"

namespace cui::vulkan {

vk_device_manager vk_device_manager::s_device_manager;

vk_device::vk_device(const vkc_device_create_data &data) {
  // Creates 3 queues graphic, compute and transfer
  m_name = data.device_name;
  // TODO:
  m_queue_priorities[0] = 1;
  m_queue_priorities[1] = 1;
  m_queue_priorities[2] = 1;

  std::vector<VkDeviceQueueCreateInfo> queue_create_info;
  std::vector<uint32_t> requested_queues;

  for (const queue_info &info : data.queues) {
    if (info.second == UINT32_MAX)
      continue;
    m_queues[info.first] = queue{VK_NULL_HANDLE, info.second, VK_NULL_HANDLE};
    if (std::find(requested_queues.begin(), requested_queues.end(),
                  info.second) == requested_queues.end()) {
      queue_create_info.emplace_back(
          vkcDeviceQueueCreateInfo(0, 1, &m_queue_priorities[0], info.second));
      requested_queues.emplace_back(info.second);
    }
  }

  VkDeviceCreateInfo device_create_info =
      vkcDeviceCreateInfo(queue_create_info, VK_NULL_HANDLE,
                          data.gpu_extensions, data.features_chain);
  VK_CHECK(vkCreateDevice(vk_graphic_device::get().get_device(),
                          &device_create_info, nullptr, &m_device),
           "Error in vulkan function vkCreateDevice.");

  // Creates command pools and queues
  for (auto &[name, obj] : m_queues) {

    // Locate queue
    vkGetDeviceQueue(m_device, obj.queue_index, 0, &obj.queue);

    // Create pool
    VkCommandPoolCreateInfo pool_create_info =
        vkcCommandPoolCreateInfo(obj.queue_index);
    VK_CHECK(vkCreateCommandPool(m_device, &pool_create_info, nullptr,
                                 &obj.command_pool) != VK_SUCCESS,
             "Error in vulkan function vkCreateCommandPool.");
  }
}

void vk_device::create_fence(VkFenceCreateFlags flags) {
  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = flags;
  VK_CHECK(vkCreateFence(m_device, &fenceInfo, nullptr, &m_fence),
           "Error creating fence in: " + m_name);
}

VkCommandBuffer
vk_device::create_command_buffers(const VkCommandBufferLevel bufferLevel,
                                  const uint32_t bufferCount,
                                  const VkCommandPool pool) const {
  ASSERT(bufferCount == 0,
         "The count of commands buffer created can't be cero.",
         TEXT_COLOR_ERROR);

  VkCommandBuffer commandBuffer;
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = pool;
  allocInfo.level = bufferLevel;
  allocInfo.commandBufferCount = bufferCount;

  VK_CHECK(vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer),
           "Error creating command buffer...");
  return commandBuffer;
}

VkCommandBuffer vk_device::create_one_time_use_command_buffer(
    const VkCommandBufferLevel &bufferLevel, const std::string &pool_name) {
  VkCommandBuffer cmd =
      create_command_buffers(bufferLevel, 1, m_queues[pool_name].command_pool);
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vkBeginCommandBuffer(cmd, &beginInfo);
  return cmd;
}

void vk_device::submit_command_buffer(VkCommandBuffer *cmd,
                                      const uint32_t count,
                                      const std::string &pool_name,
                                      const uint32_t semaphores_count,
                                      const VkSemaphore *semaphores) {
  for (uint32_t i = 0; i < count; i++)
    vkEndCommandBuffer(cmd[i]);

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = count;
  submitInfo.pCommandBuffers = cmd;
  submitInfo.waitSemaphoreCount = semaphores_count;
  submitInfo.pSignalSemaphores = (semaphores == nullptr) ? nullptr : semaphores;
  vkQueueSubmit(m_queues[pool_name].queue, 1, &submitInfo, VK_NULL_HANDLE);
}

void vk_device::wait_to_finish(const std::string pool_name) {
  vkQueueWaitIdle(m_queues[pool_name].queue);
}

void vk_device::free() {
  for (auto &[name, obj] : m_queues)
    vkDestroyCommandPool(m_device, obj.command_pool, nullptr);
  if (m_fence != VK_NULL_HANDLE)
    vkDestroyFence(m_device, m_fence, nullptr);
  vkDestroyDevice(m_device, nullptr);
}
} // namespace cui::vulkan
