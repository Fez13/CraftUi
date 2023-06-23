#pragma once
#include "../vendor/glm/glm.hpp"
#include "../vulkan/vk_buffer.h"
#include "material.h"
#include "mesh.h"

#include "pipelines/pipeline_label.h"
#include <unordered_map>
#include <vector>

/*
  @brief This is a experimental implementation of using a matrix to decrease the
  amount of draw calls, potentially saving space in the vram.
  If it's false it will use old fashion draw calls vector, which will be
  deprecated if the matrix works.

*/
#define MAP_FOR_DRAW_CALLS false

namespace cui::renderer {

struct mesh_draw_calls {
  uint32_t count;
  std::vector<const material *> materials;
  std::vector<const glm::mat4 *> transforms; //<- upload to gpu
};

struct draw_call {
  const mesh *c_mesh;
  const glm::mat4 *c_transform;
  const material *c_material;
};

class scene {
public:
  uint32_t create_draw_call(const mesh *mesh,
                        const pipeline_info::pipeline_label pipeline,
                        const material *material, const glm::mat4 *mat);
                        
  void get_draw_calls(const pipeline_info::pipeline_label pipeline,
                      VkDrawIndexedIndirectCommand *const draw_calls,
                      glm::mat4 *const matrices, int *const material_index,uint32_t *draw_call_count);

  mesh_manager scene_mesh_manager;

private:
#if MAP_FOR_DRAW_CALLS == true
  std::unordered_map<pipeline_info::pipeline_label,
                     std::unordered_map<const mesh *, mesh_draw_calls>>
      m_draw_calls;
#else
  std::unordered_map<pipeline_info::pipeline_label, std::vector<draw_call>>
      m_draw_calls;
#endif
};

scene *get_current_scene();

void create_default_scene();

} // namespace cui::renderer
