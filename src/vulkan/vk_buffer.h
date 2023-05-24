#pragma once
#include "vk_device.h"
#include "vk_utils.h"

namespace cui::vulkan {
/*
@brief Default conenction between host and device memory via shader buffers.
*/
class vk_buffer {

public:
  explicit vk_buffer() = default;

  vk_buffer(vk_device *device, const uint32_t size,
            const VkBufferUsageFlags usage, const VkSharingMode sharing_mode);

  /*
  @brief Handles all memory initialization, from finding the memory to
  allocating it.
  @warning If the buffer constructor hasn't been called, it will crash.
  */
  void initialize_buffer_memory(const VkMemoryPropertyFlags memory_properties);

  /*
  @brief Returns a pointer to the location of the memory either on cpu or gpu.
  If its been called before without unmapping it it will reuse the pointer.
  @todo Memory sync for not VK_MEMORY_PROPERTY_HOST_COHERENT_BIT buffers
  @warning if the buffer doesn't contains the flag
  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT reading might be wrong and writings might
  not complete.
  @tparam T The class of the memory inside the direction.
  @param validate Will compare sizeof(T) and the allocated buffer memory and if
  they are not multiples it will crash.
  */
  template <class T> T *get_memory_location(const bool validate = false) {
    ASSERT((m_size % sizeof(T) != 0 && validate),
           "Error size of T and size of allocated memory are not compatible.",
           TEXT_COLOR_ERROR);
    if (m_mapped_memory != nullptr)
      return static_cast<T *>(m_mapped_memory);
    void *data;
    vkMapMemory(m_device->get_device(), m_memory, 0, m_size, 0, &data);
    m_mapped_memory = data;
    return static_cast<T *>(data);
  }

  /*
    @brief Disconnects host a device memory, allows for some operations to be
    done with gpu meory.
    @warning If the memory isn't already mapped it will crash.
  */
  void unmap_memory();

  VkBuffer get_buffer() const { return m_buffer; };

  /*
    @brief Returns the address of the buffer in a uint64 structure.
    @warning Non-predictable behavior if the buffer or memory are not
    initialized.
  */
  VkDeviceAddress get_address() const;

  /*
  @brief Copies data form another buffer into this buffer.
  @warning Nullptr or any of the buffers not being initialized will crash. Also
  the this buffer must have the VK_BUFFER_USAGE_TRANSFER_SRC_BIT flag. And the
  other VK_BUFFER_USAGE_TRANSFER_DST_BIT.
  */
  void copy_from(const vk_buffer *buffer);

  /*
  @brief Copies data form this buffer into a image.
  @warning Nullptr, this buffer, or the image not being initialized will crash.
  Also the buffer must have the VK_BUFFER_USAGE_TRANSFER_SRC_BIT flag.
  @param image The image.
  @param size A 3 component vector of the size of the image
  */
  void copy_to_image(VkImage &image, const glm::uvec3 size);

  VkBufferUsageFlags get_usage() const { return m_usages; }

  void free();

  size_t get_size() const { return m_size; };

private:
  size_t m_size = 0;
  uint32_t m_memory_index = 0;
  vk_device *m_device = nullptr;
  VkBuffer m_buffer = nullptr;
  VkBufferUsageFlags m_usages;
  VkDeviceMemory m_memory = nullptr;
  VkMemoryRequirements m_memory_requirements;
  VkMemoryPropertyFlags m_memory_properties;
  void *m_mapped_memory = nullptr;
};

} // namespace cui::vulkan