#pragma once

#include "../vendor/glm/glm.hpp"
#include "../vulkan/vulkan.h"
#include <iostream>
#include <vector>

namespace cui::renderer {

struct vertex_2d {
  glm::vec2 position;
  glm::vec2 texture_position;
  vertex_2d(const glm::vec2 position, const glm::vec2 texture_position) : position(position),texture_position(texture_position){}  
  static VkVertexInputBindingDescription get_binding_description();

  static std::vector<VkVertexInputAttributeDescription>
  get_attribute_description();

  inline static const int attribute_count = 2;
};

struct vertex_3d {
  glm::vec3 position;
  glm::vec2 texture_position;
  glm::vec3 normal;

  static VkVertexInputBindingDescription get_binding_description();

  static std::vector<VkVertexInputAttributeDescription>
  get_attribute_description();

  inline static const int attribute_count = 3;
};

} // namespace cui::renderer
