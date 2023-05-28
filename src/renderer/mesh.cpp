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
} // namespace cui::renderer
