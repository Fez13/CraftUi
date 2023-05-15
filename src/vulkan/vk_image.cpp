#include "vk_image.h"

namespace cui::vulkan {

void create_image(vkc_image_create_data &data, vk_device *device) {
  VkPhysicalDevice gpu = vk_graphic_device::get().get_device();

  VkImageCreateInfo image_info = vkcImageCreateInfo(data);

  VK_CHECK(
      vkCreateImage(device->get_device(), &image_info, nullptr, &data.image),
      "Fail creating a image...");

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(device->get_device(), data.image,
                               &memRequirements);

  VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
  vkGetPhysicalDeviceMemoryProperties(gpu, &deviceMemoryProperties);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = find_memory(
      data.properties, deviceMemoryProperties, memRequirements.memoryTypeBits);

  VK_CHECK(vkAllocateMemory(device->get_device(), &allocInfo, nullptr,
                            &data.image_memory),
           "Fail creating an image memory...");

  vkBindImageMemory(device->get_device(), data.image, data.image_memory, 0);
}

vk_image::vk_image(const void *data, int x, int y,
                   const std::string &device_name,
                   VkImageTiling tiling = VK_IMAGE_TILING_LINEAR)
    : m_tiling(tiling) {}

vk_image::vk_image(const std::string path, const std::string device_name,
                   const VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL)
    : m_tiling(tiling) {

  stbi_uc *image_data = stbi_load(path.c_str(), &m_size_x, &m_size_y,
                                  &m_texture_channels_count, STBI_rgb_alpha);

  ASSERT(image_data, "Error reading image path. path: " + path,
         TEXT_COLOR_ERROR);

  vk_buffer image_data_gpu(
      vk_device_manager::get().get_device(device_name), m_size_x * m_size_y * 4,
      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE);

  image_data_gpu.initialize_buffer_memory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                          VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  void *image_data_gpu_p = image_data_gpu.get_memory_location<void *>();
  memcpy(image_data_gpu_p, image_data, image_data_gpu.get_size());
  image_data_gpu.unmap_memory();

  stbi_image_free(image_data);
  create_image(image_data_gpu);
  image_data_gpu.free();
}

void vk_image::create_image(vk_buffer &buffer) {}
  
  
  
  
} // namespace cui::vulkan
