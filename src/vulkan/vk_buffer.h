#pragma once
#include "vk_device.h"
#include "vk_utils.h"

namespace cui::vulkan {
class vk_buffer {

public:
  explicit vk_buffer() = default;

  vk_buffer(vk_device *device, const uint32_t &size,
            const VkBufferUsageFlags &usage, VkSharingMode sharing_mode);

  void
  create_buffer_memory_requirements(const VkMemoryPropertyFlags &memory_properties,
                                 const VkPhysicalDevice &physical_device);

  void allocate_memory();

  template <class T> T *get_memory_location() {
    void *data;
    vkMapMemory(m_mainDevice->device, m_memory, 0, m_size, 0, &data);
    return static_cast<T *>(data);
  }

  void unmap_memory();

  VkBuffer get_buffer() const;

  VkDeviceAddress get_address() const;

  void copy_from(vk_buffer* buffer);

  void free();

  void bindBuffer();

  size_t getSize() const;

private:
  
  

};

} // namespace cui::vulkan
