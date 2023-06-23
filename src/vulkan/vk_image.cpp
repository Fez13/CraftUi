#include "vk_image.h"

namespace cui::vulkan {

void transition_image_layout(vk_device *device, VkImage &image,
                             const VkFormat format,
                             const VkImageLayout old_layout,
                             const VkImageLayout new_layout) {
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
                       vk_device *device, VkImageView &image_view,
                       VkImage &image, const VkImageAspectFlags aspect_flags,
                       const uint32_t baseMinp, const uint32_t base_array,
                       const uint32_t level_count, const uint32_t layer_count) {
  VkImageViewCreateInfo image_create_info =
      vkcImageViewCreateInfo(view_type, image, format, aspect_flags, baseMinp,
                             base_array, level_count, layer_count);
  VK_CHECK(vkCreateImageView(device->get_device(), &image_create_info, nullptr,
                             &image_view),
           "Fail creating a image view...");
}

void create_image_sampler(VkSampler &sampler, VkDevice device,
                          VkFilter magFilter, VkFilter minFilter,
                          VkBool32 normalizeCordinates, VkBool32 compareOp,
                          sampler_mode SMD, VkBorderColor borderColor) {
  const VkPhysicalDevice gpu = vk_graphic_device::get().get_device();
  VkSamplerCreateInfo samplerInfo{};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;

  samplerInfo.addressModeU = SMD.U;
  samplerInfo.addressModeV = SMD.V;
  samplerInfo.addressModeW = SMD.W;

  samplerInfo.anisotropyEnable = VK_FALSE; // TODO:
  samplerInfo.maxAnisotropy = 1.0f;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(gpu, &properties);

  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = borderColor;
  samplerInfo.unnormalizedCoordinates = normalizeCordinates;

  samplerInfo.compareEnable = compareOp;
  samplerInfo.compareOp = VK_COMPARE_OP_NEVER;

  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 1.0f;
  VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &sampler),
           "Fail creating an image sampler...");
}

vk_image::vk_image(const void *data, int x, int y,
                   const std::string &device_name, const VkImageTiling tiling,
                   const VkFormat format)
    : m_tiling(tiling), m_format(format) {
  m_device = vk_device_manager::get().get_device(device_name);
  m_size_x = x;
  m_size_y = y;

  vk_buffer stageBuffer =
      vk_buffer(m_device, x * y * 4,
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);
  stageBuffer.initialize_buffer_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  
  void *gpu_memory = stageBuffer.get_memory_location<void*>();
  memcpy(gpu_memory, data, static_cast<size_t>(stageBuffer.get_size()));
  stageBuffer.unmap_memory();

  fill_image(stageBuffer);
  stageBuffer.free();
}

vk_image::vk_image(const std::string path, const std::string device_name,
                   const VkImageTiling tiling, const VkFormat format)
    : m_tiling(tiling), m_format(format) {
  m_device = vk_device_manager::get().get_device(device_name);
  stbi_uc *image_data = stbi_load(path.c_str(), &m_size_x, &m_size_y,
                                  &m_texture_channels_count, STBI_rgb_alpha);

  ASSERT(!image_data, "Error reading image path. path: " + path,
         TEXT_COLOR_ERROR);

  vk_buffer image_data_gpu(m_device, m_size_x * m_size_y * 4,
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

  transition_image_layout(m_device, m_image, m_format,
                          VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  buffer.copy_to_image(m_image, {m_size_x, m_size_y, 1});

  transition_image_layout(m_device, m_image, m_format,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  create_image_view(VK_IMAGE_VIEW_TYPE_2D, m_format, m_device, m_image_view,
                    m_image, VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1, 1);
}

vk_image::vk_image(const std::string device_name, const VkImageTiling tiling,
                   const VkFormat format, const VkImageUsageFlagBits usage,
                   const VkImageAspectFlagBits aspect, const int x, const int y)
    : m_tiling(tiling), m_format(format), m_usage(usage), m_size_x(x),
      m_size_y(y) {
  m_device = vk_device_manager::get().get_device(device_name);
  create_image();

  create_image_view(VK_IMAGE_VIEW_TYPE_2D, m_format, m_device, m_image_view,
                    m_image, VK_IMAGE_ASPECT_DEPTH_BIT, 0, 0, 1, 1);
}

void vk_image::create_image() {
  VkPhysicalDevice gpu = vk_graphic_device::get().get_device();

  VkImageCreateInfo image_info =
      vkcImageCreateInfo({m_size_x, m_size_y}, m_format, m_tiling, m_usage,
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
  allocInfo.memoryTypeIndex =
      find_memory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, deviceMemoryProperties,
                  memRequirements.memoryTypeBits);

  VK_CHECK(
      vkAllocateMemory(m_device->get_device(), &allocInfo, nullptr, &m_memory),
      "Fail creating an image memory...");

  vkBindImageMemory(m_device->get_device(), m_image, m_memory, 0);
}

void vk_image::free() const {
  ASSERT(m_dependent,
         "Error, an dependent image has tried to be freed before it's "
         "dependency. This can cause unexpected behaviour.",
         TEXT_COLOR_ERROR);
  vkDestroyImage(m_device->get_device(), m_image, nullptr);
  vkDestroyImageView(m_device->get_device(), m_image_view, nullptr);
  vkFreeMemory(m_device->get_device(), m_memory, nullptr);
}

} // namespace cui::vulkan
