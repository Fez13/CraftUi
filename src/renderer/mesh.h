#pragma once

#include "../vendor/glm/glm.hpp"
#include "../vendor/tinyobjloader/tiny_obj_loader.h"
#include "../vulkan/vulkan.h"
#include "vertex.h"
#include <iostream>
#include <vector>
namespace cui::renderer {

struct geometry_2d {
  std::vector<uint32_t> indices;
  std::vector<vertex_2d> vertices;
  
  geometry_2d(const std::vector<vertex_2d>&,const std::vector<uint32_t>&);
  
  void check_state();
  
};

struct geometry_3d {
  std::vector<uint32_t> indices;
  std::vector<vertex_3d> vertices;
  
  geometry_3d(const std::vector<vertex_3d>&,const std::vector<uint32_t>&);
  geometry_3d(const std::string path);
  
  void check_state();
};

class mesh {

  // TODO:
  // Mesh manager
  // Mesh instance
};
} // namespace cui::renderer
