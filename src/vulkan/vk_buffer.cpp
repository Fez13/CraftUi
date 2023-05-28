#include "vk_buffer.h"

namespace cui::vulkan {
vk_buffer::vk_buffer(vk_device *device, const size_t size,
                     const VkBufferUsageFlags usage,
                     const VkSharingMode sharing_mode)
    : m_device(device), m_size(size), m_usages(usage) {
  VkBufferCreateInfo create_buffer =
      vkcBufferCreateInfo(sharing_mode, size, usage);
  VK_CHECK(vkCreateBuffer(m_device->get_device(), &create_buffer, nullptr,
                          &m_buffer),
           "Error creating buffer of size: " + std::to_string(size));
  m_memory_requirements.size = m_size;
}

void vk_buffer::initialize_buffer_memory(
    const VkMemoryPropertyFlags memory_properties) {
  ASSERT(m_device == nullptr,
         "Error, device not valid or buffer constructor hasn't been called.",TEXT_COLOR_ERROR);
  // Gets appropiate memory
  m_memory_properties = memory_properties;

  VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
  vkGetPhysicalDeviceMemoryProperties(vk_graphic_device::get().get_device(),
                                      &deviceMemoryProperties);

  m_memory_index = find_memory(m_memory_properties, deviceMemoryProperties, 1);

  vkGetBufferMemoryRequirements(m_device->get_device(), m_buffer,
                                &m_memory_requirements);
  // Allocates memory
  VkMemoryAllocateInfo memory_allocate_info =
      vkcMemoryAllocateInfo(m_memory_index, m_memory_requirements.size);

  VK_CHECK(vkAllocateMemory(m_device->get_device(), &memory_allocate_info,
                            nullptr, &m_memory),
           "Error allocating gpu buffer");
  vkBindBufferMemory(m_device->get_device(), m_buffer, m_memory, 0);
}

void vk_buffer::unmap_memory() {
  ASSERT(m_mapped_memory == nullptr, "Error, tried to unmap already unmapped memory.",TEXT_COLOR_ERROR);
  vkUnmapMemory(m_device->get_device(), m_memory);
  m_mapped_memory = nullptr;
}

VkDeviceAddress vk_buffer::get_address() const {
  VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
  bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
  bufferDeviceAI.buffer = m_buffer;
  return 0;
  // TODO:
  // return vulkanFunctions::pvkGetBufferDeviceAddressKHR(m_mainDevice->device,
  // &bufferDeviceAI);
}

void vk_buffer::copy_from(const vk_buffer *buffer) {

  if (!(buffer->get_usage() & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
    LOG("Wen writing in a gpu buffer the source must have the "
        "'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' flag, the operation will be "
        "aborted.",
        TEXT_COLOR_ERROR);
    return;
  }
  if (!(m_usages & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
    LOG("Wen writing in a gpu buffer the destine must have the "
        "'VK_BUFFER_USAGE_TRANSFER_DST_BIT' flag, the operation will be "
        "aborted.",
        TEXT_COLOR_ERROR);
    return;
  }

  VkCommandBuffer cmd = m_device->create_one_time_use_command_buffer(
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, "TRANSFER");
  VkBufferCopy bufferCopy{};
  bufferCopy.dstOffset = 0;
  bufferCopy.srcOffset = 0;
  bufferCopy.size = m_size;

  vkCmdCopyBuffer(cmd, buffer->get_buffer(), m_buffer, 1, &bufferCopy);
  m_device->submit_command_buffer(&cmd, 1, "TRANSFER");
  m_device->wait_to_finish("TRANSFER");
}

void vk_buffer::free() {
  if(m_mapped_memory != nullptr)
    unmap_memory();
  vkDestroyBuffer(m_device->get_device(), m_buffer, nullptr);
  vkFreeMemory(m_device->get_device(), m_memory, nullptr);
}

void vk_buffer::copy_to_image(VkImage &image, const glm::uvec3 size) {
  if (!(m_usages & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)) {
    LOG("Wen writing in a gpu buffer/image the source must have the "
        "'VK_BUFFER_USAGE_TRANSFER_SRC_BIT' flag, the operation will be "
        "aborted.",
        TEXT_COLOR_ERROR);
    return;
  }

  VkCommandBuffer cmd = m_device->create_one_time_use_command_buffer(
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, "TRANSFER");

  VkBufferImageCopy region{};
  region.bufferOffset = 0;
  region.bufferRowLength = 0;
  region.bufferImageHeight = 0;
  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = {0, 0, 0};
  region.imageExtent = {size.x, size.y, size.z};

  vkCmdCopyBufferToImage(cmd, m_buffer, image,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

  m_device->submit_command_buffer(&cmd, 1, "TRANSFER");
  m_device->wait_to_finish("TRANSFER");
}

} // namespace cui::vulkan
