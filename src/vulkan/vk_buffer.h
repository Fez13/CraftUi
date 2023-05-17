#pragma once
#include "vk_device.h"
#include "vk_utils.h"

namespace cui::vulkan {
class vk_buffer {

public:
  explicit vk_buffer() = default;

  vk_buffer(vk_device *device, const uint32_t size,
            const VkBufferUsageFlags usage,const VkSharingMode sharing_mode);

  // This will handel creation of requirements, allocation and binding.
  void initialize_buffer_memory(const VkMemoryPropertyFlags memory_properties);

  template <class T> T *get_memory_location() {
    void *data;
    vkMapMemory(m_device->get_device(), m_memory, 0, m_size, 0, &data);
    return static_cast<T *>(data);
  }

  void unmap_memory();

  VkBuffer get_buffer() const { return m_buffer; };

  VkDeviceAddress get_address() const;

  void copy_from(vk_buffer *buffer);
  
  void copy_to_image(VkImage &image, const glm::uvec3 size);

  VkBufferUsageFlags get_usage() const { return m_usages; }

  void free();

  size_t get_size() const{return m_size;};

private:
  size_t m_size = 0;
  uint32_t m_memory_index = 0;
  vk_device *m_device = nullptr;
  VkBuffer m_buffer = nullptr;
  VkBufferUsageFlags m_usages;
  VkDeviceMemory m_memory = nullptr;
  VkMemoryRequirements m_memory_requirements;
  VkMemoryPropertyFlags m_memory_properties;
};

} // namespace cui::vulkan
