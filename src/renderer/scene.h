#pragma once
#include "material.h"
#include "pipelines/pipeline_label.h"
#include "../vulkan/vk_buffer.h"
#include "../vendor/glm/glm.hpp"
#include <unordered_map>
#include <vector>

namespace cui::renderer {

struct mesh_draw_calls {
  uint32_t count;
  std::vector<material *> textures;
  std::vector<glm::mat4 *> transforms; //<- upload to gpu
  std::vector<vulkan::vk_buffer *> special_data;
};

class scene {
  // TODO: Make public fields private
public:
  
  std::unordered_map<pipeline_info::pipeline_label, std::vector<mesh_draw_calls>> m_draw_calls;
  
};
} // namespace cui::renderer
