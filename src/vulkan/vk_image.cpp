#include "vk_image.h"

namespace cui::vulkan {

void transition_image_layout(vk_device *device, VkImage image, VkFormat format,
                             VkImageLayout old_layout,
                             VkImageLayout new_layout) {
  VkCommandBuffer cmd = device->create_one_time_use_command_buffer(
      VK_COMMAND_BUFFER_LEVEL_PRIMARY, "GRAPHIC");
  VkImageMemoryBarrier image_barrier =
      vkcImageMemoryBarrier(old_layout, new_layout, image);

  VkPipelineStageFlags source_stage;
  VkPipelineStageFlags destination_stage;

  if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
      new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    image_barrier.srcAccessMask = 0;
    image_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    image_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    image_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    throw std::invalid_argument("unsupported layout transition!");
  }

  vkCmdPipelineBarrier(cmd, source_stage, destination_stage, 0, 0, nullptr, 0,
                       nullptr, 1, &image_barrier);
  device->submit_command_buffer(&cmd, 1, "GRAPHIC");
  device->wait_to_finish("GRAPHIC");
}

void create_image_view(const VkImageViewType view_type, const VkFormat format,
                       vk_device *device, VkImageView image_view, VkImage image,
                       const VkImageAspectFlags aspect_flags,
                       const uint32_t baseMinp, const uint32_t base_array,
                       const uint32_t level_count, const uint32_t layer_count) {
  VkImageViewCreateInfo image_create_info =
      vkcImageViewCreateInfo(view_type, image, format, aspect_flags, baseMinp,
                             base_array, level_count, layer_count);
  VK_CHECK(vkCreateImageView(device->get_device(), &image_create_info, nullptr,
                             &image_view),
           "Fail creating a image view...");
}

vk_image::vk_image(const void *data, int x, int y,
                   const std::string &device_name, VkImageTiling tiling)
    : m_tiling(tiling) {
  m_device = vk_device_manager::get().get_device(device_name);

  // TODO
}

vk_image::vk_image(const std::string path, const std::string device_name,
                   const VkImageTiling tiling)
    : m_tiling(tiling) {
  m_device = vk_device_manager::get().get_device(device_name);
  stbi_uc *image_data = stbi_load(path.c_str(), &m_size_x, &m_size_y,
                                  &m_texture_channels_count, STBI_rgb_alpha);

  ASSERT(image_data, "Error reading image path. path: " + path,
         TEXT_COLOR_ERROR);

  vk_buffer image_data_gpu(m_device, m_size_x * m_size_y * m_texture_channels_count,
                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                           VK_SHARING_MODE_EXCLUSIVE);

  image_data_gpu.initialize_buffer_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  void *image_data_gpu_p = image_data_gpu.get_memory_location<void *>();
  memcpy(image_data_gpu_p, image_data, image_data_gpu.get_size());
  image_data_gpu.unmap_memory();

  stbi_image_free(image_data);
  fill_image(image_data_gpu);
  image_data_gpu.free();
}

void vk_image::fill_image(vk_buffer &buffer) {

  create_image();

  transition_image_layout(m_device, m_image, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  buffer.copy_to_image(m_image, {m_size_x, m_size_y, 1});

  transition_image_layout(m_device, m_image, VK_FORMAT_R8G8B8A8_SRGB,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  create_image_view(VK_IMAGE_VIEW_TYPE_2D, VK_FORMAT_R8G8B8A8_SRGB, m_device,
                    m_image_view, m_image, VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1,
                    1);
}

void vk_image::create_image() {
  VkPhysicalDevice gpu = vk_graphic_device::get().get_device();

  VkImageCreateInfo image_info = vkcImageCreateInfo(
      {m_size_x, m_size_y}, VK_FORMAT_R8G8B8A8_SRGB, m_tiling,
      VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
      VK_IMAGE_LAYOUT_UNDEFINED);

  VK_CHECK(
      vkCreateImage(m_device->get_device(), &image_info, nullptr, &m_image),
      "Fail creating a image...");

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(m_device->get_device(), m_image,
                               &memRequirements);

  VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
  vkGetPhysicalDeviceMemoryProperties(gpu, &deviceMemoryProperties);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = find_memory(
      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, deviceMemoryProperties, memRequirements.memoryTypeBits);

  VK_CHECK(vkAllocateMemory(m_device->get_device(), &allocInfo, nullptr,
                            &m_memory),
           "Fail creating an image memory...");

  vkBindImageMemory(m_device->get_device(), m_image, m_memory, 0);
}

void vk_image::free() const {
  vkDestroyImage(m_device->get_device(), m_image, nullptr);
  vkDestroyImageView(m_device->get_device(), m_image_view, nullptr);
  vkFreeMemory(m_device->get_device(), m_memory, nullptr);
}

} // namespace cui::vulkan
