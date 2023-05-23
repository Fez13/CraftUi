#pragma once
#include "vk_buffer.h"
#include "vk_device.h"
#include "vk_image.h"
#include "vk_utils.h"
#include <unordered_map>

namespace cui::vulkan {

struct vk_descriptor_set_array;

/*
  @brief It's the connection between cpu and gpu in terms of buffers.
  @param update All functions started with update will bind some type of buffer
  with a shader, they must be called after initialization.
  @param create All functions started with create will create a bind of some
  type of buffer with a shader, they must be called before initialization.

*/
class vk_descriptor_set {
public:
  vk_descriptor_set(vk_device *device);
  static vk_descriptor_set *create(vk_device *device);

  VkDescriptorSet get_descriptor_set() { return m_descriptor_set; }
  VkDescriptorSet *get_descriptor_set_p() { return &m_descriptor_set; }

  void initialize_layout();

  VkDescriptorSetLayout get_layout() { return m_descriptor_set_layout; }

  /*
    @brief Shall be called after initialization to allow the descriptor to be
    used.
    @warning Multiple calls or being call before initialization will create
    unexpected behaviour.
  */
  void allocate_descriptor_set();

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
  static void free_one(vk_descriptor_set *object);

private:
  void free() const;

  bool m_locked = false;
  std::unordered_map<uint32_t, descriptor_binding> m_bindings;

  VkDescriptorPool m_descriptor_set_pool;
  VkDescriptorSet m_descriptor_set;
  VkDescriptorSetLayout m_descriptor_set_layout;
  vk_device *m_device;

  static std::vector<vk_descriptor_set> s_instancies;
  friend struct vk_descriptor_set_array;
};

struct vk_descriptor_set_array {
public:
  vk_descriptor_set_array(vk_device *device) : m_device(device) {}

  /*
    @brief Will bind all descriptor sets of the array to the pipeline.
    @warning Not initialized descriptors, arrays or pipelines will create unexpected behavior and warnings.
  */
  void bind_all(const VkPipelineLayout &layout, VkCommandBuffer &cmd) {
    VkDescriptorSet *descriptor_sets;
    descriptor_sets =
        (VkDescriptorSet *)std::malloc(sizeof(VkDescriptorSet) * m_data.size());

    for (uint32_t i = 0; i < m_data.size(); i++) {
      descriptor_sets[i] = m_data[i]->get_descriptor_set();
    }
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0,
                            m_data.size(), descriptor_sets, 0, nullptr);
    std::free(descriptor_sets);
  }

  vk_descriptor_set *get(const uint32_t index) { return m_data[index]; }

  vk_descriptor_set *create() {
    vk_descriptor_set *obj = vk_descriptor_set::create(m_device);
    m_data.emplace_back(obj);
    return obj;
  };

  void add(vk_descriptor_set *object) {
    if (std::count(m_data.begin(), m_data.end(), object)) {
      LOG("vk_descriptor_set_array already contains object",
          TEXT_COLOR_WARNING);
    } else {
      m_data.push_back(object);
    }
  }

  std::vector<VkDescriptorSetLayout> get_layouts() const {
    std::vector<VkDescriptorSetLayout> array;
    array.resize(m_data.size());
    for (uint32_t i = 0; i < m_data.size(); i++) {
      array[i] = m_data[i]->get_layout();
    }
    return array;
  }

  uint32_t count() const { return m_data.size(); }
  bool is_lock() { return lock; }
  void lock_array() {
    m_descriptors.resize(m_data.size());
    for (uint32_t i = 0; i < m_data.size(); i++) {
      m_descriptors[i] = m_data[i]->get_descriptor_set();
    }
    lock = true;
  }

  std::vector<VkDescriptorSet> &get_descriptor_array() { return m_descriptors; }

  void free() {
    for (uint32_t i = 0; i < m_data.size(); i++) {
      m_data[i]->free();
    }
    m_descriptors.clear();
    m_data.clear();
  }

private:
  bool lock = false;
  vk_device *m_device;
  std::vector<VkDescriptorSet> m_descriptors;
  std::vector<vk_descriptor_set *> m_data;
};

} // namespace cui::vulkan
