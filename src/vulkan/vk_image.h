#pragma once

#include "vk_utils.h"

namespace cui::vulkan {

class vk_image {
public:
  VkImage get_image() const {return m_image;};
  VkImageView get_image_view() const {return m_image_view;};

private:
  VkImage m_image;
  VkImageView m_image_view;
};

} // namespace cui::vulkan
