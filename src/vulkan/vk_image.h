#pragma once
#include "vk_utils.h"

#include "vk_buffer.h"
#include "vk_device.h"

#include "../vendor/stb/stb_image.h"

namespace cui::vulkan {

void create_image(vkc_image_create_data &data,vk_device* device);

class vk_image {
public:
  vk_image() = default;

  vk_image(const void *data, int x, int y, const std::string &device_name,
           VkImageTiling tiling = VK_IMAGE_TILING_LINEAR);

  vk_image(const std::string path, const std::string device_name,
           const VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL);

  VkImage get_image() const { return m_image; };
  VkImageView get_image_view() const { return m_image_view; };

private:
  void create_image(vk_buffer &buffer);

  int m_texture_channels_count;
  int m_size_x;
  int m_size_y;
  VkImageTiling m_tiling;
  VkImage m_image;
  VkDeviceMemory m_memory;
  VkImageView m_image_view;
  vk_device *device;
};
} // namespace cui::vulkan
