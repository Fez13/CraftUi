#pragma once
#include "vk_buffer.h"
#include "vk_device.h"
#include "vk_utils.h"
#include "vk_image.h"
#include <unordered_map>

namespace cui::vulkan {

class vk_descriptor_set {
public:
  vk_descriptor_set(vk_device *device);
  static vk_descriptor_set *create(vk_device *device);

  VkDescriptorSet get_descriptor_set() { return m_descriptor_set; }


  void initialize_layout();

  void allocate_descriptor_set();

  //
  // Bindings
  //

  // Create
  void create_binding_uniform_buffer(
      uint32_t binding,
      const VkShaderStageFlagBits stages = VK_SHADER_STAGE_VERTEX_BIT,
      const uint32_t buffer_count = 1);

  void create_binding_storage_buffer(
      uint32_t binding,
      const VkShaderStageFlagBits stages = VK_SHADER_STAGE_VERTEX_BIT,
      const uint32_t buffer_count = 1);

  void create_binding_sampled_image(
      uint32_t binding,
      const VkShaderStageFlagBits stages = VK_SHADER_STAGE_VERTEX_BIT,
      const uint32_t image_count = 1);

  // Update
  void update_binding_uniform_buffer(const uint32_t index,
                                     const vk_buffer *buffers,
                                     const uint32_t count = 1);
  void update_binding_storage_buffer(const uint32_t index,
                                     const vk_buffer *buffers,
                                     const uint32_t count);
  void update_binding_sampled_image(const uint32_t index,
                                    const vk_image *images,
                                    const uint32_t count);

  static void free_all(){};
  static void free_one(vk_descriptor_set* object);
  
private:

  void free() const;
  
  bool m_locked = false;
  std::unordered_map<uint32_t, descriptor_binding> m_bindings;

  VkDescriptorPool m_descriptor_set_pool;
  VkDescriptorSet m_descriptor_set;
  VkDescriptorSetLayout m_descriptor_set_layout;
  vk_device *m_device;

  static std::vector<vk_descriptor_set> s_instancies;
};

} // namespace cui::vulkan
