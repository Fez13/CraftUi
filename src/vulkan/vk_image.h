#pragma once
#include "vk_utils.h"

#include "vk_buffer.h"
#include "vk_device.h"

#include "../vendor/stb/stb_image.h"

namespace cui::vulkan {

void create_image_view(const VkImageViewType view_type, const VkFormat format,
                       vk_device *device, VkImageView &image_view,
                       VkImage &image, const VkImageAspectFlags aspect_flags,
                       const uint32_t baseMinp, const uint32_t base_array,
                       const uint32_t level_count, const uint32_t layer_count);
class vk_image {
public:
  vk_image() = default;

  vk_image(const void *data, int x, int y, const std::string &device_name,
           const VkImageTiling tiling = VK_IMAGE_TILING_LINEAR,
           const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

  vk_image(const std::string path, const std::string device_name,
           const VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL,
           const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB);

  vk_image(const std::string device_name, const VkImageTiling tiling,
           const VkFormat format, const VkImageUsageFlagBits usage,
           const VkImageAspectFlagBits aspect, const int x, const int y);

  VkImage get_image() const { return m_image; }
  VkImageView get_image_view() const { return m_image_view; }
  VkFormat get_format() const{return m_format;}
  
  void free() const;

private:
  void fill_image(vk_buffer &buffer);
  void create_image();

  int m_texture_channels_count;
  int m_size_x;
  int m_size_y;
  VkImageUsageFlags m_usage =
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  VkFormat m_format;
  VkImageTiling m_tiling;
  VkImage m_image;
  VkDeviceMemory m_memory;
  VkImageView m_image_view;
  vk_device *m_device;
};
} // namespace cui::vulkan
