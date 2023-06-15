#pragma once
#include "../core/macros.h"
#include "../vendor/glm/glm.hpp"
#include "../vulkan/vk_buffer.h"
#include "../vulkan_config/vulkan_config.hpp"
#include "../vulkan/vk_descriptor_set.h"
#include "../vulkan/vk_device.h"
#include "../vulkan/vk_image.h"

namespace cui::renderer {

struct material_data {
  uint32_t albedo_texture_index = 0;
  uint32_t metallic_texture_index = 0;
  uint32_t normal_texture_index = 0;
  float metallic_strength = 1;
  glm::vec4 albedo_color{1, 1, 1, 1};
};

struct material {
public:
  void create_material(const uint32_t albedo_texture = 0,
                       const uint32_t metallic_texture = 0,
                       const uint32_t normal_texture = 0);

  const uint32_t get_index() const{
    if (m_populated)
      return m_material_index;
    else {
      LOG("Warning, a uninitialized material has been requested, default "
          "material is going to be returned",
          TEXT_COLOR_WARNING);
    }
    return 0;
  }

  uint32_t &get_albedo_texture() {
    return s_mapped_data[m_material_index].albedo_texture_index;
  }

  uint32_t &get_metallic_texture() {
    return s_mapped_data[m_material_index].metallic_texture_index;
  }

  float &get_metallic_strength() {
    return s_mapped_data[m_material_index].metallic_strength;
  }

  // float& getMetallicSmoothness(){return
  // s_mappedData[m_materialIndex].metallicSmoothness;}

  uint32_t &get_normal_texture() {
    return s_mapped_data[m_material_index].normal_texture_index;
  }

  glm::vec4 &get_albedo_color() {
    return s_mapped_data[m_material_index].albedo_color;
  }

  static void create_material_buffer(const std::string deviceName);

  static void bind_default_buffer(vulkan::vk_descriptor_set *instance,
                                  const uint32_t index);

  static void create_default_material();

  /*
       The material system takes a few binds in the descriptor sets...
        Sampler: 0
        Images array: 1
        Materials array: 2
       TODO: bindigs 2 and 3 are still work to do
      */
  static void
  create_descriptor_set_locations(vulkan::vk_descriptor_set *descriptorSet);

  static material &get_default() { return s_default_material; }

  static void free_buffer() { s_materials_buffer.free(); }

  static vulkan::vk_descriptor_set* set_descriptor_positions(vulkan::vk_descriptor_set_array* dsta);

  explicit material();

private:
  bool m_populated = false;
  uint32_t m_material_index = 0;

  inline static uint32_t s_material_count = 0;
  inline static material_data *s_mapped_data = nullptr;
  inline static vulkan::vk_buffer s_materials_buffer = vulkan::vk_buffer();
  static material s_default_material;
};

} // namespace cui::renderer
