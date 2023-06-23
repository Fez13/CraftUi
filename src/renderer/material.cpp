#include "material.h"

namespace cui::renderer {

void material::create_descriptor_set_locations(
    vulkan::vk_descriptor_set *descriptorSet) {
  descriptorSet->create_binding_sampler(0, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
  descriptorSet->create_binding_sampled_image(1, VK_SHADER_STAGE_FRAGMENT_BIT,
                                              MAX_TEXTURE_COUNT);
  descriptorSet->create_binding_uniform_buffer(2, VK_SHADER_STAGE_ALL);
}

void material::create_material(const int albedo_texture,
                               const int metallic_texture,
                               const int normal_texture) {
  s_mapped_data[m_material_index].albedo_texture_index = albedo_texture;
  s_mapped_data[m_material_index].albedo_color = {1, 1, 1, 1};

  s_mapped_data[m_material_index].metallic_texture_index = metallic_texture;
  // s_mappedData[m_materialIndex].metallicSmoothness = 1; //TODO
  s_mapped_data[m_material_index].albedo_color_strength = 1;

  s_mapped_data[m_material_index].normal_texture_index = normal_texture;
  m_populated = true;
}

material material::s_default_material;

material::material() {
  m_material_index = s_material_count;
  s_material_count++;
}

void material::create_default_material() {
  s_default_material = material();
  s_default_material.create_material();
}

/*
  @warning This function will assume that the descriptor set was populated by
  material::create_descriptor_set_locations if it wasn't it will crash.
*/
void material::bind_default_buffer(vulkan::vk_descriptor_set *instance) {
  instance->update_binding_uniform_buffer(2, &s_materials_buffer);
  instance->update_binding_sampler(0, &s_image_sampler,1);
  
}

void material::free_buffer() {
  for (auto &image : s_images) {
    image->m_dependent = false;
  }
  s_materials_buffer.free();
}

int material::add_image(vulkan::vk_image *image) {
  image->m_dependent = true;
  s_images.emplace_back(image);
  return s_images.size() - 1;
}

void material::bind_image_array(vulkan::vk_descriptor_set *set) {
  set->update_binding_sampled_image(1, *s_images.data(), s_images.size());
}

void material::create_material_buffer(const std::string deviceName) {

  s_materials_buffer = vulkan::vk_buffer(
      vulkan::vk_device_manager::get().get_device(deviceName),
      sizeof(material_data) * MAX_MATERIAL_COUNT,
      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);

  s_materials_buffer.initialize_buffer_memory(
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT); // VK_MEMORY_PROPERTY_HOST_CACHED_BIT

  s_mapped_data = s_materials_buffer.get_memory_location<material_data>();

  vulkan::create_image_sampler(
      s_image_sampler,
      vulkan::vk_device_manager::get().get_device(deviceName)->get_device(),
      VK_FILTER_LINEAR, VK_FILTER_LINEAR, VK_FALSE, VK_TRUE);
}

vulkan::vk_descriptor_set *
material::set_descriptor_positions(vulkan::vk_descriptor_set_array *dsta) {
  vulkan::vk_descriptor_set *dst = dsta->create();
  dst->create_binding_sampler(0, VK_SHADER_STAGE_ALL, 1); // Default sampler
  dst->create_binding_sampled_image(1, VK_SHADER_STAGE_ALL,
                                    MAX_TEXTURE_COUNT); // Image array
  dst->create_binding_uniform_buffer(2, VK_SHADER_STAGE_ALL,
                                     1); // Material array
  dst->create_binding_uniform_buffer(3, VK_SHADER_STAGE_ALL,
                                     1); // Material index array
  return dst;
}

} // namespace cui::renderer
