#include "mesh.h"

namespace cui::renderer {

geometry_2d::geometry_2d(const std::vector<vertex_2d> &_vertices,
                         const std::vector<uint32_t> &_indices)
    : vertices(_vertices), indices(_indices) {
  check_state();
}

geometry_3d::geometry_3d(const std::vector<vertex_3d> &_vertices,
                         const std::vector<uint32_t> &_indices)
    : vertices(_vertices), indices(_indices) {
  check_state();
}

geometry_3d::geometry_3d(const std::string path) {

  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  std::string err;

  ASSERT(!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str()),
         "Error loading model path: " + path + "\n\t Errors: " + err,
         TEXT_COLOR_ERROR);

  for (uint32_t i = 0; i < shapes.size(); i++) {
    for (uint32_t j = 0; j < shapes[i].mesh.indices.size(); j++) {

      vertices.emplace_back();
      vertices.back().position = {
          attrib.vertices[3 * shapes[i].mesh.indices[j].vertex_index + 0],
          attrib.vertices[3 * shapes[i].mesh.indices[j].vertex_index + 1],
          attrib.vertices[3 * shapes[i].mesh.indices[j].vertex_index + 2]};

      vertices.back().normal = {
          attrib.normals[3 * shapes[i].mesh.indices[j].normal_index + 0],
          attrib.normals[3 * shapes[i].mesh.indices[j].normal_index + 1],
          attrib.normals[3 * shapes[i].mesh.indices[j].normal_index + 2]};

      vertices.back().texture_position = {
          attrib.texcoords[2 * shapes[i].mesh.indices[j].texcoord_index + 0],
          1.0f - attrib.texcoords[2 * shapes[i].mesh.indices[j].texcoord_index +
                                  1]};

      indices.push_back(indices.size());
    }
  }

  check_state();
}

void geometry_2d::check_state() {
  const uint32_t max = *std::max_element(indices.begin(), indices.end());

  if (max == 0)
    LOG("Indices of a geometry are zero", TEXT_COLOR_WARNING);

  if (max >= vertices.size())
    LOG("Indices of a geometry are higher then the count of vertices",
        TEXT_COLOR_WARNING);
}

void geometry_3d::check_state() {
  const uint32_t max = *std::max_element(indices.begin(), indices.end());

  if (max == 0)
    LOG("Indices of a geometry are zero", TEXT_COLOR_WARNING);

  if (max >= vertices.size())
    LOG("Indices of a geometry are higher then the count of vertices",
        TEXT_COLOR_WARNING);
}

void mesh_manager::bind_indexes(VkCommandBuffer &buffer) {
  vkCmdBindIndexBuffer(buffer, m_index_buffer.get_buffer(), 0, INDEX_TYPE_VULKAN);
}

void mesh_manager::bind_vertices_2d(VkCommandBuffer &buffer,
                                    const VkDeviceSize *offset) {
  ASSERT(m_vertex_buffer_array_2d[0] == nullptr,
         "A 2D vertex buffer has tried to be binded, however there "
         "is not any mesh in the buffer. Remember to load your "
         "meshes and crate a buffer!.",
         TEXT_COLOR_ERROR);
  vkCmdBindVertexBuffers(buffer, 0, 1, m_vertex_buffer_array_2d.data(), offset);
}

void mesh_manager::bind_vertices_3d(VkCommandBuffer &buffer,
                                    const VkDeviceSize *offset) {
  ASSERT(m_vertex_buffer_array_3d[0] == nullptr,
         "A 3D vertex buffer has tried to be binded, however there "
         "is not any mesh in the buffer. Remember to load your "
         "meshes and crate a buffer!.",
         TEXT_COLOR_ERROR);
  vkCmdBindVertexBuffers(buffer, 0, 1, m_vertex_buffer_array_3d.data(), offset);
}

void mesh_manager::create_mesh(mesh *mesh, geometry_2d *geometry) {
  mesh->m_geometry = pipeline_info::CUI_GEOMETRY_TYPE_2D;
  m_2d_queue.push_back({mesh, geometry});
}

void mesh_manager::create_mesh(mesh *mesh, geometry_3d *geometry) {
  mesh->m_geometry = pipeline_info::CUI_GEOMETRY_TYPE_3D;
  m_3d_queue.push_back({mesh, geometry});
}

void mesh_manager::initialize(vulkan::vk_device *device) {
  ASSERT(m_initialized,
         "Error, initialize was called two times in a mesh_manager this should "
         "not happen and may be an error.",
         TEXT_COLOR_ERROR);
  m_device = device;
  m_initialized = true;

  for (auto &[mesh, geometry] : m_3d_queue) {
    mesh->m_indices_count = m_index_count;
    mesh->m_vertices_start = m_vertex_count_3d;
    mesh->m_indices_count = geometry->indices.size();
    mesh->m_vertices_count = geometry->vertices.size();
    m_index_count += geometry->indices.size();
    m_vertex_count_3d += geometry->vertices.size();
  }

  for (const auto &[mesh, geometry] : m_2d_queue) {
    mesh->m_indices_count = m_index_count;
    mesh->m_vertices_start = m_vertex_count_2d;
    mesh->m_indices_count = geometry->indices.size();
    mesh->m_vertices_count = geometry->vertices.size();
    m_index_count += geometry->indices.size();
    m_vertex_count_2d += geometry->vertices.size();
  }

  size_t index_memory_size =
      static_cast<size_t>(sizeof(uint32_t) * m_index_count);
  std::vector<uint32_t> indices_array;
  indices_array.reserve(m_index_count);

  size_t vertices_3d_memory_size =
      static_cast<size_t>(sizeof(vertex_3d) * m_vertex_count_3d);
  std::vector<vertex_3d> vertices_3d_array;
  vertices_3d_array.reserve(m_vertex_count_3d);

  size_t vertices_2d_memory_size =
      static_cast<size_t>(sizeof(vertex_2d) * m_vertex_count_2d);
  std::vector<vertex_2d> vertices_2d_array;
  vertices_2d_array.reserve(m_vertex_count_2d);

  uint32_t i_index = 0;
  uint32_t i_vertex_3d = 0;
  uint32_t i_vertex_2d = 0;

  for (auto &[mesh, geometry] : m_3d_queue) {
    for (uint32_t i = 0; i < geometry->indices.size(); i++) {
      indices_array[i_index] = geometry->indices[i];
      i_index++;
    }

    for (uint32_t i = 0; i < geometry->vertices.size(); i++) {
      vertices_3d_array[i_vertex_3d] = geometry->vertices[i];
      i_vertex_3d++;
    }
  }

  for (auto &[mesh, geometry] : m_2d_queue) {
    for (uint32_t i = 0; i < geometry->indices.size(); i++) {
      indices_array[i_index] = geometry->indices[i];
      i_index++;
    }

    for (uint32_t i = 0; i < geometry->vertices.size(); i++) {
      vertices_2d_array[i_vertex_2d] = geometry->vertices[i];
      i_vertex_2d++;
    }
  }

  if (vertices_3d_memory_size != 0) {
    vulkan::vk_buffer vertices_3d_stage(device, vertices_3d_memory_size,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_SHARING_MODE_EXCLUSIVE);
    vertices_3d_stage.initialize_buffer_memory(
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_vertex_buffer_3d = vulkan::vk_buffer(
        device, vertices_3d_memory_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_SHARING_MODE_EXCLUSIVE);
    m_vertex_buffer_3d.initialize_buffer_memory(
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    std::memcpy(vertices_3d_stage.get_memory_location<vertex_3d>(),
                vertices_3d_array.data(), vertices_3d_memory_size);
    vertices_3d_array.clear();
    vertices_3d_stage.unmap_memory();

    m_vertex_buffer_3d.copy_from(&vertices_3d_stage);
    m_vertex_buffer_array_3d[0] = m_vertex_buffer_3d.get_buffer();
    vertices_3d_stage.free();
  }

  if (vertices_2d_memory_size != 0) {
    vulkan::vk_buffer vertices_2d_stage(device, vertices_2d_memory_size,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_SHARING_MODE_EXCLUSIVE);
    vertices_2d_stage.initialize_buffer_memory(
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_vertex_buffer_2d = vulkan::vk_buffer(
        device, vertices_2d_memory_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_SHARING_MODE_EXCLUSIVE);
    m_vertex_buffer_2d.initialize_buffer_memory(
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    std::memcpy(vertices_2d_stage.get_memory_location<vertex_2d>(),
                vertices_2d_array.data(), vertices_2d_memory_size);
    vertices_2d_array.clear();
    vertices_2d_stage.unmap_memory();

    m_vertex_buffer_2d.copy_from(&vertices_2d_stage);
    m_vertex_buffer_array_2d[0] = m_vertex_buffer_2d.get_buffer();
    vertices_2d_stage.free();
  }

  if (index_memory_size != 0) {
    vulkan::vk_buffer indices_stage(device, index_memory_size,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_SHARING_MODE_EXCLUSIVE);
    indices_stage.initialize_buffer_memory(
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_index_buffer = vulkan::vk_buffer(
        device, index_memory_size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_SHARING_MODE_EXCLUSIVE);
    m_index_buffer.initialize_buffer_memory(
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    std::memcpy(indices_stage.get_memory_location<INDEX_TYPE>(),
                indices_array.data(), vertices_2d_memory_size);
    indices_array.clear();
    indices_stage.unmap_memory();

    m_index_buffer.copy_from(&indices_stage);
    indices_stage.free();
  } else {
    LOG("Warning a mesh manager without indices was initialized",
        TEXT_COLOR_WARNING);
  }
}

} // namespace cui::renderer
