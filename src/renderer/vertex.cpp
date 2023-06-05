#include "vertex.h"

namespace cui::renderer {

VkVertexInputBindingDescription vertex_3d::get_binding_description() {
  VkVertexInputBindingDescription binding_description{};
  binding_description.binding = 0;
  binding_description.stride = sizeof(vertex_3d);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return binding_description;
}

std::vector<VkVertexInputAttributeDescription>
vertex_3d::get_attribute_description() {
  std::vector<VkVertexInputAttributeDescription> attribute_description;
  attribute_description.resize(attribute_count);

  attribute_description[0].binding = 0;
  attribute_description[0].location = 0;
  attribute_description[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_description[0].offset = offsetof(vertex_3d, position);

  attribute_description[1].binding = 0;
  attribute_description[1].location = 2;
  attribute_description[1].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_description[1].offset = offsetof(vertex_3d, texture_position);

  attribute_description[2].binding = 0;
  attribute_description[2].location = 1;
  attribute_description[2].format = VK_FORMAT_R32G32B32_SFLOAT;
  attribute_description[2].offset = offsetof(vertex_3d, normal);

  return attribute_description;
}

VkVertexInputBindingDescription vertex_2d::get_binding_description() {
  VkVertexInputBindingDescription binding_description{};
  binding_description.binding = 0;
  binding_description.stride = sizeof(vertex_2d);
  binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  return binding_description;
}

std::vector<VkVertexInputAttributeDescription>
vertex_2d::get_attribute_description() {
  std::vector<VkVertexInputAttributeDescription> attribute_description;
  attribute_description.resize(attribute_count);

  attribute_description[0].binding = 0;
  attribute_description[0].location = 0;
  attribute_description[0].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_description[0].offset = offsetof(vertex_2d, position);

  attribute_description[1].binding = 0;
  attribute_description[1].location = 2;
  attribute_description[1].format = VK_FORMAT_R32G32_SFLOAT;
  attribute_description[1].offset = offsetof(vertex_2d, texture_position);
  return attribute_description;
}

} // namespace cui::renderer
