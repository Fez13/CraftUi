#include "vk_descriptor_set.h"
namespace cui::vulkan {

std::vector<vk_descriptor_set> vk_descriptor_set::s_instancies = {};

vk_descriptor_set::vk_descriptor_set(vk_device *device) : m_device(device) {}

vk_descriptor_set *vk_descriptor_set::create(vk_device *device) {
  s_instancies.push_back(vk_descriptor_set(device));
  return &s_instancies[s_instancies.size() - 1];
}

void vk_descriptor_set::initialize_layout() {
  const uint32_t binding_count = m_bindings.size();

  std::vector<VkDescriptorSetLayoutBinding> bindings_layout;
  bindings_layout.resize(binding_count);

  std::vector<VkDescriptorPoolSize> sizes;
  sizes.resize(binding_count);

  uint32_t i = 0;
  for (const auto &[key, binding] : m_bindings) {
    bindings_layout[i] = binding.layout_binding;
    sizes[i].descriptorCount = binding.count;
    sizes[i].type = binding.type;
    i++;
  }

  VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create =
      vkcDescriptorSetLayoutCreateInfo(binding_count, bindings_layout.data());

  VK_CHECK(vkCreateDescriptorSetLayout(m_device->get_device(),
                                       &descriptor_set_layout_create, nullptr,
                                       &m_descriptor_set_layout),
           "Error creating descriptor set layout.");

  VkDescriptorPoolCreateInfo descriptor_pool_create =
      vkcDescriptorPoolCreateInfo(binding_count, sizes.data());

  VK_CHECK(vkCreateDescriptorPool(m_device->get_device(),
                                  &descriptor_pool_create, nullptr,
                                  &m_descriptor_set_pool),
           "Error creating DescriptorPool.");
}

void vk_descriptor_set::allocate_descriptor_set() {
  VkDescriptorSetAllocateInfo allocate_info = vkcDescriptorSetAllocateInfo(
      &m_descriptor_set_layout, m_descriptor_set_pool);

  VK_CHECK(vkAllocateDescriptorSets(m_device->get_device(), &allocate_info,
                                    &m_descriptor_set),
           "Error allocating a descriptorSet.");
}

void vk_descriptor_set::create_binding_uniform_buffer(
    uint32_t binding, const VkShaderStageFlagBits stages,
    const uint32_t buffer_count) {
  ASSERT(m_locked,
         "Tried to add binding to locked descriptor. Add the bindings first "
         "and after call initialize_layout().",
         TEXT_COLOR_ERROR);

  ASSERT(m_bindings.contains(binding),
         "Error, this binding is already in use, binding: " +
             std::to_string(binding),
         TEXT_COLOR_ERROR);

  m_bindings[binding].layout_binding = vkcDescriptorSetLayoutBinding(
      binding, buffer_count, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr,
      stages);
  m_bindings[binding].count = buffer_count;
  m_bindings[binding].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
}

void vk_descriptor_set::create_binding_storage_buffer(
    uint32_t binding, const VkShaderStageFlagBits stages,
    const uint32_t buffer_count) {
  ASSERT(m_locked,
         "Tried to add binding to locked descriptor. Add the bindings first "
         "and after call initialize_layout().",
         TEXT_COLOR_ERROR);

  ASSERT(m_bindings.contains(binding),
         "Error, this binding is already in use, binding: " +
             std::to_string(binding),
         TEXT_COLOR_ERROR);

  m_bindings[binding].layout_binding = vkcDescriptorSetLayoutBinding(
      binding, buffer_count, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr,
      stages);
  m_bindings[binding].count = buffer_count;
  m_bindings[binding].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
}

void vk_descriptor_set::create_binding_sampled_image(
    uint32_t binding, const VkShaderStageFlagBits stages,
    const uint32_t image_count) {
  ASSERT(m_locked,
         "Tried to add binding to locked descriptor. Add the bindings first "
         "and after call initialize_layout().",
         TEXT_COLOR_ERROR);

  ASSERT(m_bindings.contains(binding),
         "Error, this binding is already in use, binding: " +
             std::to_string(binding),
         TEXT_COLOR_ERROR);

  m_bindings[binding].layout_binding = vkcDescriptorSetLayoutBinding(
      binding, image_count, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, nullptr, stages);
  m_bindings[binding].count = image_count;
  m_bindings[binding].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
}

void vk_descriptor_set::create_binding_sampler(
    uint32_t binding,
    const VkShaderStageFlagBits stages,
    const uint32_t sampler_count) {
  ASSERT(m_locked,
         "Tried to add binding to locked descriptor. Add the bindings first "
         "and after call initialize_layout().",
         TEXT_COLOR_ERROR);

  ASSERT(m_bindings.contains(binding),
         "Error, this binding is already in use, binding: " +
             std::to_string(binding),
         TEXT_COLOR_ERROR);

  m_bindings[binding].layout_binding = vkcDescriptorSetLayoutBinding(
      binding, sampler_count, VK_DESCRIPTOR_TYPE_SAMPLER, nullptr, stages);
  m_bindings[binding].count = sampler_count;
  m_bindings[binding].type = VK_DESCRIPTOR_TYPE_SAMPLER;
}

void vk_descriptor_set::update_binding_uniform_buffer(const uint32_t index,
                                                      const vk_buffer *buffers,
                                                      const uint32_t count) {
  ASSERT(!m_bindings.contains(index),
         "A buffer has tried to be binded to index: " + std::to_string(index) +
             ",however, this position is not \n\t being used by the "
             "Descriptor...",
         TEXT_COLOR_ERROR);

  ASSERT(m_bindings[index].type != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
         "Error, tried to update non uniform binding with uniform data.",
         TEXT_COLOR_ERROR);

  std::vector<VkDescriptorBufferInfo> buffer_infos;
  buffer_infos.resize(count);

  for (uint32_t i = 0; i < count; i++) {
    buffer_infos[0].buffer = buffers[i].get_buffer();
    buffer_infos[0].offset = 0;
    buffer_infos[0].range = buffers[i].get_size();
  }

  VkWriteDescriptorSet write_descriptor_set;
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.descriptorType = m_bindings[index].type;
  write_descriptor_set.dstBinding = index;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorCount = buffer_infos.size();
  write_descriptor_set.dstSet = m_descriptor_set;
  write_descriptor_set.pNext = nullptr;
  write_descriptor_set.pBufferInfo = buffer_infos.data();
  vkUpdateDescriptorSets(m_device->get_device(), 1, &write_descriptor_set, 0,
                         nullptr);
}

void vk_descriptor_set::update_binding_storage_buffer(const uint32_t binding,
                                                      const vk_buffer *buffers,
                                                      const uint32_t count) {
  ASSERT(
      !m_bindings.contains(binding),
      "A buffer has tried to be binded to index: " + std::to_string(binding) +
          ",however, this position is not \n\t being used by the "
          "Descriptor...",
      TEXT_COLOR_ERROR);

  ASSERT(m_bindings[binding].type != VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
         "Error, tried to update non uniform binding with uniform data.",
         TEXT_COLOR_ERROR);

  std::vector<VkDescriptorBufferInfo> buffer_infos;
  buffer_infos.resize(count);

  for (uint32_t i = 0; i < count; i++) {
    buffer_infos[0].buffer = buffers[i].get_buffer();
    buffer_infos[0].offset = 0;
    buffer_infos[0].range = buffers[i].get_size();
  }

  VkWriteDescriptorSet write_descriptor_set;
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.descriptorType = m_bindings[binding].type;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorCount = buffer_infos.size();
  write_descriptor_set.dstSet = m_descriptor_set;
  write_descriptor_set.pNext = nullptr;
  write_descriptor_set.pBufferInfo = buffer_infos.data();
  vkUpdateDescriptorSets(m_device->get_device(), 1, &write_descriptor_set, 0,
                         nullptr);
}

void vk_descriptor_set::update_binding_sampled_image(const uint32_t binding,
                                                     const vk_image *images,
                                                     const uint32_t count) {
  ASSERT(!m_bindings.contains(binding),
         "A image has tried to be binded to index: " + std::to_string(binding) +
             ",however, this position is not \n\t being used by the current "
             "descriptor...",
         TEXT_COLOR_ERROR);

  std::vector<VkDescriptorImageInfo> image_info;
  image_info.resize(count);

  for (uint32_t i = 0; i < image_info.size(); ++i) {
    image_info[i].sampler = nullptr;
    image_info[i].imageView = images[i].get_image_view();
    image_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }

  VkWriteDescriptorSet write_descriptor_set{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.descriptorType = m_bindings[binding].type;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorCount = count;
  write_descriptor_set.dstSet = m_descriptor_set;
  write_descriptor_set.pImageInfo = image_info.data();
  vkUpdateDescriptorSets(m_device->get_device(), 1, &write_descriptor_set, 0,
                         nullptr);
}

void vk_descriptor_set::update_binding_sampler(const uint32_t binding,
                                               const VkSampler *samplers,
                                               const uint32_t count) {
  ASSERT(!m_bindings.contains(binding),
         "A sampler has tried to be binded to index: " + std::to_string(binding) +
             ",however, this position is not \n\t being used by the current "
             "descriptor...",
         TEXT_COLOR_ERROR);

  std::vector<VkDescriptorImageInfo> sampler_info;
  sampler_info.resize(count);

  for (uint32_t i = 0; i < sampler_info.size(); ++i) {
    sampler_info[i].imageView = nullptr;
    sampler_info[i].sampler = samplers[i];
    sampler_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  }
  
  VkWriteDescriptorSet write_descriptor_set{};
  write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write_descriptor_set.descriptorType = m_bindings[binding].type;
  write_descriptor_set.dstBinding = binding;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorCount = count;
  write_descriptor_set.dstSet = m_descriptor_set;
  write_descriptor_set.pImageInfo = sampler_info.data();
  vkUpdateDescriptorSets(m_device->get_device(), 1, &write_descriptor_set, 0,
                         nullptr);
}

void vk_descriptor_set::free() const {
  vkDestroyDescriptorPool(m_device->get_device(), m_descriptor_set_pool,
                          nullptr);
  vkDestroyDescriptorSetLayout(m_device->get_device(), m_descriptor_set_layout,
                               nullptr);
}

void vk_descriptor_set::free_one(vk_descriptor_set *object) {
  object->free();

  uint32_t i = 0;
  for (auto &obj : s_instancies) {
    if (object == &obj) {
      s_instancies.erase(s_instancies.begin() + i);
    }
    i++;
  }
}

} // namespace cui::vulkan
