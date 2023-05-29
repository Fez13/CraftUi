#pragma once

#include "../vendor/glm/glm.hpp"
#include "../vendor/tinyobjloader/tiny_obj_loader.h"
#include "../vulkan/vulkan.h"
#include "../vulkan_config/vulkan_config.hpp"
#include "vertex.h"
#include <iostream>
#include <vector>
namespace cui::renderer {

using geometry_type = uint8_t;
enum : geometry_type {
  CUI_GEOMETRY_TYPE_3D = 0,
  CUI_GEOMETRY_TYPE_2D = 1,
  CUI_GEOMETRY_TYPE_NULL = 255
};

struct geometry_2d {
  std::vector<INDEX_TYPE> indices;
  std::vector<vertex_2d> vertices;

  geometry_2d(const std::vector<vertex_2d> &, const std::vector<INDEX_TYPE> &);

  void check_state();
};

struct geometry_3d {
  std::vector<INDEX_TYPE> indices;
  std::vector<vertex_3d> vertices;

  geometry_3d(const std::vector<vertex_3d> &, const std::vector<INDEX_TYPE> &);
  geometry_3d(const std::string path);

  void check_state();
};

class mesh_manager;
class mesh {
public:
  const bool is_initialized() const { return m_initialized; }
  const geometry_type get_geometry() const { return m_geometry; }
  const uint32_t get_vertices_start() const { return m_vertices_start; }
  const uint32_t get_vertices_count() const { return m_vertices_count; }
  const uint32_t get_indices_start() const { return m_indices_start; }
  const uint32_t get_indices_count() const { return m_indices_count; }

private:
  geometry_type m_geometry = CUI_GEOMETRY_TYPE_NULL;
  uint32_t m_vertices_start = 0;
  uint32_t m_vertices_count = 0;
  uint32_t m_indices_start = 0;
  uint32_t m_indices_count = 0;
  bool m_initialized = false;
  friend class mesh_manager;
};

class mesh_manager {

public:
  void bind_indexes(VkCommandBuffer &buffer);

  void bind_vertices_2d(VkCommandBuffer &buffer, const VkDeviceSize *offset);

  void bind_vertices_3d(VkCommandBuffer &buffer, const VkDeviceSize *offset);

  mesh create_mesh(mesh *mesh, geometry_2d *geometry);
  mesh create_mesh(mesh *mesh, geometry_3d *geometry);

  void initialize(vulkan::vk_device* device);

private:
  bool m_initialized = false;
  std::vector<std::pair<mesh *, geometry_3d *>> m_3d_queue;
  std::vector<std::pair<mesh *, geometry_2d *>> m_2d_queue;

  uint32_t m_vertex_count_2d = 0;
  uint32_t m_vertex_count_3d = 0;
  uint32_t m_index_count = 0;

  vulkan::vk_buffer m_vertex_buffer_3d;
  vulkan::vk_buffer m_vertex_buffer_2d;
  vulkan::vk_buffer m_index_buffer;
  std::array<VkBuffer, 1> m_vertex_buffer_array_2d{nullptr};
  std::array<VkBuffer, 1> m_vertex_buffer_array_3d{nullptr};
  vulkan::vk_device* m_device = nullptr;
};

} // namespace cui::renderer